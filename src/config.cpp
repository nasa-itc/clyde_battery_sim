#include <boost/property_tree/json_parser.hpp>
#include "config.hpp"

using namespace itc::battery;

Config::Config(const std::string &cfg_file, uint8_t i2c_address) : log_level(), nos(), i2c_address(i2c_address)
{
    load(cfg_file);
}

void Config::load(const std::string &cfg_file)
{
    if (cfg_file.empty())
        return;

    boost::property_tree::ptree cfg;
    boost::property_tree::read_json(cfg_file, cfg);
    
    log_level = cfg.get("log_level", "error");

    nos.uri = cfg.get("nos.uri", "");
    nos.node = cfg.get("nos.node", "");
    nos.i2c_bus = cfg.get("nos.i2c_bus", "");
}
