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

#ifndef _BAT_CONFIG_HPP_
#define _BAT_CONFIG_HPP_

#include <string>
#include <cstdint>
#include <boost/property_tree/ptree.hpp>

namespace itc
{
    namespace battery
    {
        /**
         * \brief NOS engine config
         */
        struct NosConfig
        {
            std::string uri;     //!< nos server uri
            std::string node;    //!< nos node name
            std::string i2c_bus; //!< nos i2c hardware bus name
        };

        /**
         * \brief battery simulator config
         */
        class Config
        {
        public:
            /**
             * \brief Constructor
             *
             * \param cfgfile battery config file
             */
            Config(const std::string& cfgfile = "", uint8_t i2c_address = 0);

            /**
             * \brief Load the battery config file
             *
             * \param cfgfile battery config file
             */
            void load(const std::string& cfgfile);
            
        public:
            std::string log_level; //!< log level
            NosConfig nos;         //!< nos engine config
            uint8_t i2c_address; //!< battery i2c base address
        };

    }
}

#endif
