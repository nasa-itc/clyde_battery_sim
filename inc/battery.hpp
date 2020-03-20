/* Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
   limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
   for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
   any warranty that the software will be error free.


   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
   arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,

   contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
   documentation or services provided hereunder

   ITC Team
   NASA IV&V
   ivv-itc@lists.nasa.gov
*/

#ifndef _BAT_SIM_HPP_
#define _BAT_SIM_HPP_

#include <I2C/Client/I2CSlave.hpp>
#include <cstdint>

#include "config.hpp"

namespace itc
{
    namespace battery
    {

        const uint16_t TLM_VBAT = 0xE280; //!< battery output voltage
        const uint16_t TLM_IBAT = 0xE284; //!< battery current magnitude
        const uint16_t TLM_IDIRBAT = 0xE28E; //!< battery current direction
        const uint16_t TLM_TBRD = 0xE308; //!< motherboard temperature
        const uint16_t TLM_IPCM5V = 0xE214; //!< current draw of 5V bus
        const uint16_t TLM_VPCM5V = 0xE210; //!< output voltage of 5V bus
        const uint16_t TLM_IPCM3V3 = 0xE204; //!< current draw of 3.3V bus
        const uint16_t TLM_VPCM3V3 = 0xE200; //!< output voltage of 3.3V bus
        const uint16_t TLM_TBAT1 = 0xE398; //!< daughterboard 1 temperature
        const uint16_t TLM_HBAT1 = 0xE39F; //!< daughterboard 1 heater status
        const uint16_t TLM_TBAT2 = 0xE3A8; //!< daughterboard 2 temperature
        const uint16_t TLM_HBAT2 = 0xE3AF; //!< daughterboard 2 heater status
        const uint16_t TLM_TBAT3 = 0xE3B8; //!< daughterboard 3 temperature
        const uint16_t TLM_HBAT3 = 0xE3BF; //!< daughterboard 3 heater status
        const uint16_t TLM_TBAT4 = 0xE3C8; //!< daughterboard 4 temperature
        const uint16_t TLM_HBAT4 = 0xE3CF; //!< daughterboard 4 heater status

        /**
         * \brief Battery Simulator
         */
        class Battery : public NosEngine::I2C::I2CSlave
        {
        public:
            /**
             * \brief Constructor
             *
             * \param config IMU simulator config
             */
            Battery(const Config &config, uint8_t id);

            /**
             * \brief Destructor
             */
            virtual ~Battery();

            /*
             * \brief I2C master read
             *
             * \param rbuf Read data buffer
             * \param rlen Read data buffer length
             *
             * \return Number of bytes read
             */
            size_t i2c_read(uint8_t* rbuf, size_t rlen);

            /*
             * \brief I2C master write
             *
             * \param wbuf Write data buffer
             * \param wlen Write data buffer length
             *
             * \return Number of bytes written
             */
            size_t i2c_write(const uint8_t *wbuf, size_t wlen);

            uint16_t get_board_status(void);
            uint16_t get_last_error(void);
            uint16_t get_version(void);
            uint16_t get_checksum(void);
            uint16_t get_telemetry(const uint8_t *channel);
            uint16_t get_num_bor(void);
            uint16_t get_num_asr(void);
            uint16_t get_num_mr(void);
            uint16_t get_heater_controller_status(void);
            uint16_t set_heater_controller_status(uint8_t status);
            void manual_reset(void);

        private:
            uint8_t battery_id; //!< battery id
            uint16_t response; //!< i2c response
            uint16_t last_error; //!< last error that occured
            uint16_t board_status; //!< battery board status
            uint16_t version; //!< battery version
            uint16_t checksum; //!< battery checksum
            uint16_t bor; //!< number of brownout resets
            uint16_t asr; //!< number of automatic software resets
            uint16_t mr; //!< number of manual resets
            uint16_t heater_controller_status; //!< heater controller status
            

        };
    }
}

#endif
