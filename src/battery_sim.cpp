#include <iostream>
#include <string>
#include <csignal>
#include <thread>
#include <chrono>

#include <ItcLogger/Logger.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "battery.hpp"
#include "config.hpp"

namespace
{
    bool quit = false;
    ItcLogger::Logger *logger = ItcLogger::Logger::get("bat_logger");
}

void handle_signal(int sig)
{
    quit = true;
}

bool parse_command_line(int argc, char *argv[], std::string &cfg_file)
{
    namespace bpo = boost::program_options;

    bpo::variables_map battery_options;
    bpo::options_description battery_description("STF-1 Battery Simulator Options");
    battery_description.add_options()
        ("help,h", "display help")
        ("config,c", bpo::value<std::string>(&cfg_file)->default_value("batteries.json"), "batteries config file (json)");

    try
    {
        bpo::store(bpo::parse_command_line(argc, argv, battery_description), battery_options);
        bpo::notify(battery_options);
    }
    catch (const bpo::error &e)
    {
        std::cerr << "battery command line parse error: " << e.what() << std::endl;
        return false;
    }

    if (battery_options.count("help"))
    {
        std::cout << battery_description;
        return false;
    }

    return true;
}

/* configure logger */
void configure_logger(const std::string& level)
{
    // create default stdio logger
    logger->set_level(ItcLogger::String2Level(level.c_str()));
    ItcLogger::TargetPtr target(new ItcLogger::Target(STDIO_BUILTIN_TARGET_IMPL, ItcLogger::LogArguments()));
    target->set_format("%t [%l] %s");
    logger->add_target(target);
}

bool get_battery_i2c_address(const std::string &cfg_file, uint8_t addresses[])
{
    if (cfg_file.empty())
        return false;

    boost::property_tree::ptree cfg;
    boost::property_tree::read_json(cfg_file, cfg);

    addresses[0] = cfg.get<uint8_t>("battery_01.address", 0);
    addresses[1] = cfg.get<uint8_t>("battery_02.address", 0);

    return true;
}

int main(int argc, char* argv[])
{
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::string cfg_file;
    if (!parse_command_line(argc, argv, cfg_file))
    {
        return 1;
    }

    uint8_t addresses[2];
    if (!get_battery_i2c_address(cfg_file, addresses))
    {
        return 1;
    }

    itc::battery::Config bat_01_config(cfg_file, addresses[0]);
    itc::battery::Config bat_02_config(cfg_file, addresses[1]);

    configure_logger(bat_01_config.log_level);
    logger->info("loaded batteries cfg %s", cfg_file.c_str());
    logger->info("creating battery #1 sim (bus=%s, addr=%#x)", bat_01_config.nos.i2c_bus.c_str(), bat_01_config.i2c_address);
    logger->info("creating battery #2 sim (bus=%s, addr=%#x)", bat_02_config.nos.i2c_bus.c_str(), bat_02_config.i2c_address);

    itc::battery::Battery sim_01(bat_01_config, 1);
    itc::battery::Battery sim_02(bat_02_config, 2);
    

    while(!quit)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    return 0;
}
