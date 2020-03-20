#include <ItcLogger/Logger.hpp>
#include "battery.hpp"

using namespace itc::battery;

ItcLogger::Logger *logger = ItcLogger::Logger::get("bat_logger");

Battery::Battery(const Config &config, uint8_t id) : NosEngine::I2C::I2CSlave(config.i2c_address, config.nos.uri, config.nos.i2c_bus), battery_id(id), response(0), last_error(0), board_status(0), version(0x1001), checksum(0xBEEF), bor(0), asr(0), mr(0), heater_controller_status(1)
{

}

Battery::~Battery(){}

size_t Battery::i2c_read(uint8_t *rbuf, size_t rlen)
{
    size_t len = rlen;

    rbuf[0] = response & 0xFF;
    rbuf[1] = (response & 0xFF00) >> 8;

    return len;
}

size_t Battery::i2c_write(const uint8_t *wbuf, size_t wlen)
{
    
    if (wlen < 2 || wlen > 3)
    {
        logger->error("Battery::i2c_write: invalid battery command");
        response = 0xFFFF;
        return 2;
    }

    response = 0;
    uint8_t command = wbuf[0];

    switch(command)
    {
    case 0x01:
        if (wbuf[1] != 0x00)
        {
            logger->error("Invalid board status data: %u expected 0x00", wbuf[1]);
            board_status |= 0x4;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = get_board_status();
        }
        break;

    case 0x03:
        if (wbuf[1] != 0x00)
        {
            logger->error("Invalid get last error data: %u expected 0x00", wbuf[1]);
            board_status |= 0x04;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = get_last_error();
        }
        break;

    case 0x04:
        if (wbuf[1] != 0x00)
        {
            logger->error("Invalid get version data: %u expected 0x00", wbuf[1]);
            board_status |= 0x04;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = get_version();
        }
        break;

    case 0x05:
        if (wbuf[1] != 0x00)
        {
            logger->error("Invalid get checksum data: %u expected 0x00", wbuf[1]);
            board_status |= 0x04;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = get_checksum();
        }

        break;

    case 0x10:
        if (wlen != 3)
        {
            logger->error("Invalid get telemetry data size: %u expected 3", wlen);
            board_status &= ~(0x04);
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = get_telemetry(&wbuf[1]);
        }
        break;

    case 0x31:
        if (wbuf[1] != 0x00)
        {
            logger->error("Invalid get BOR data: %u expected 0x00", wbuf[1]);
            board_status |= 0x04;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = get_num_bor();
        }

        break;

    case 0x32:
        if (wbuf[1] != 0x00)
        {
            logger->error("Invalid get ASR data: %u expected 0x00", wbuf[1]);
            board_status |= 0x04;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = get_num_asr();
        }
        break;

    case 0x33:
        if (wbuf[1] != 0x00)
        {
            logger->error("Invalid get MR data: %u expected 0x00", wbuf[1]);
            board_status |= 0x04;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = get_num_mr();
        }
        break;

    case 0x90:
        if (wbuf[1] != 0x00)
        {
            logger->error("Invalid get heater controller status data: %u expected 0x00", wbuf[1]);
            board_status |= 0x04;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = get_heater_controller_status();
        }
        break;

    case 0x91:
        if (wlen != 2)
        {
            logger->error("Invalid set heater controller status data length: %u expected 2", wlen);
            board_status |= 0x04;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            response = set_heater_controller_status(wbuf[1]);
        }
        break;

    case 0x80:
        if (wbuf[1] != 0x00)
        {
            logger->error("Invalid manual reset data: %u expected 0x00", wbuf[1]);
            board_status |= 0x04;
            response = 0xFFFF;
        }
        else
        {
            board_status &= ~(0x5);
            manual_reset();
            response = 0;
        }

        break;

    default:
        logger->error("Invalid command received: %u", command);
        board_status |= 0x1;
        response = 0xFFFF;
        
    }

    return wlen;
}

uint16_t Battery::get_board_status(void)
{
    logger->info("Battery: %u get board status received", battery_id);
    return board_status;
}

uint16_t Battery::get_last_error(void)
{
    logger->info("Battery: %u get last error received", battery_id);
    return last_error;
}

uint16_t Battery::get_version(void)
{
    logger->info("Battery: %u get version received", battery_id);
    return version;
}

uint16_t Battery::get_checksum(void)
{
    logger->info("Battery: %u get checksum received", battery_id);
    return checksum;
}


uint16_t Battery::get_telemetry(const uint8_t *channel)
{
    uint16_t rval = 0xFFFF;
    uint16_t tle_code = 0; 
    tle_code |= (*channel) << 8;
    tle_code |= *(channel + 1);

    logger->info("Battery: %u get telemetry received tle_code: 0x%X", battery_id, tle_code);

    switch(tle_code)
    {
    case TLM_VBAT:
        rval = 918; //V
        board_status &= ~(0x8);
        break;
    case TLM_IBAT:
        rval = 81; //mA
        board_status &= ~(0x8);
        break;
    case TLM_IDIRBAT:
        rval = 700; //ADC < 512 charging; else discharging
        board_status &= ~(0x8);
        break;
    case TLM_TBRD:
        rval = 790; //C
        board_status &= ~(0x8);
        break;
    case TLM_IPCM5V:
        rval = 757; //mA
        board_status &= ~(0x8);
        break;
    case TLM_VPCM5V:
        rval = 852; //V
        board_status &= ~(0x8);
        break;
    case TLM_IPCM3V3:
        rval = 753; //mA
        board_status &= ~(0x8);
        break;
    case TLM_VPCM3V3:
        rval = 765; //V
        board_status &= ~(0x8);
        break;
    case TLM_TBAT1:
        rval = 795; //C
        board_status &= ~(0x8);
        break;
    case TLM_HBAT1:
        rval = 680; //ADC < 512 heater off; else on
        board_status &= ~(0x8);
        break;
    case TLM_TBAT2:
        rval = 790; //C
        board_status &= ~(0x8);
        break;
    case TLM_HBAT2:
        rval = 400; //ADC < 512 heater off; else on
        board_status &= ~(0x8);
        break;
    case TLM_TBAT3:
        rval = 780; //C
        board_status &= ~(0x8);
        break;
    case TLM_HBAT3:
        rval = 450; //ADC < 512 heater off; else on
        board_status &= ~(0x8);
        break;
    case TLM_TBAT4:
        rval = 786; //C
        board_status &= ~(0x8);
        break;
    case TLM_HBAT4:
        rval = 530; //ADC < 512 heater off; else on
        board_status &= ~(0x8);
        break;
    default:
        board_status |= 0x8;
        logger->error("Invalid telemetry channel 0x%x", tle_code);
    }

    return rval;
}

uint16_t Battery::get_num_bor(void)
{
    logger->info("Battery: %u get number of brownout resets received", battery_id);
    return bor;
}

uint16_t Battery::get_num_asr(void)
{
    logger->info("Battery: %u get number of automatic software resets received", battery_id);
    return asr;
}

uint16_t Battery::get_num_mr(void)
{
    logger->info("Battery: %u get number of manual resets received", battery_id);
    return mr;
}

uint16_t Battery::get_heater_controller_status(void)
{
    logger->info("Battery: %u get heater controller status received", battery_id);
    return heater_controller_status;
}

uint16_t Battery::set_heater_controller_status(uint8_t status)
{
    logger->info("Battery: %u set heater controller status received: status %u", battery_id, status);
    if (status > 1)
    {
        logger->error("Battery: %u set heater controller status out of range", battery_id);
        return 0xFFFF;
    }
    else
    { 
        logger->info("Battery: %u set heater controller status received: status %u", battery_id, status);
        heater_controller_status = status;
    }

    return 0;
}

void Battery::manual_reset(void)
{
    logger->info("Battery: %u manual reset received", battery_id);
    mr++;
}

