/*
 * context.h
 *
 *  Created on: Marh 16, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_CONTEXT_H_
#define WEATHER_CONTEXT_H_

#include <chrono>

#include "Environment.h"
#include "lcdstrings.h"
#include "defines.h"

namespace weather {

class Context : public smachine::Environment {
public:
    Context() : version("0.9"), ip4_address("0.0.0.0"), ip6_address("00:00:00:00:00:00") {}
    virtual ~Context() {}

    std::string version;
    std::string ip4_address;
    std::string ip6_address;

    // Si7021 - I2C Humidity and Temperature Sensor
    float si7021_humidity;
    float si7021_temperature;
    float si7021_abs_humidity;

    // SPG30 - I2C Sensurion Gas Platform
    uint16_t spg30_co2;
    uint16_t spg30_tvoc;
    uint16_t spg30_base_co2;
    uint16_t spg30_base_tvoc;

    // BMP280 - I2C Digital Presure Sensor
    float bmp280_pressure;
    float bmp280_temperature;
    float bmp280_altitude;

    // TSL2561 - I2C Light-to-Digital Converter
    uint32_t tsl2651_lux;

    const float temp_C_to_F(const float temp) const {
        return temp*1.8 + 32;
    }

    //
    //Timer intervals
    //
    int measure_check_interval = MEASURE_INTERVAL;
    int measure_show_interval = MEASURE_SHOW_INTERVAL;
    int ip_check_interval = IP_CHECK_INTERVAL;

    //
    // Load/Save initial context data from/to file
    //
    // The most useful for restoring previous base values for SPG30 CO2 and TVOC
    //
    bool load_initial_data(const std::string& filename);
    bool save_initial_data(const std::string& filename);

    //Return string for UI interface
    const std::string& get_str(const int id) const {
        return _strs.get(id);
    }

private:
    LcdStrings _strs;
};

}

#endif /* WEATHER_CONTEXT_H_ */
