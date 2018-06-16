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
#include "logger.h"

namespace weather {

class Context : public smachine::Environment {
public:
    Context() : version("0.9"), ip4_address("0.0.0.0"),
        ip6_address("00:00:00:00:00:00"), _CO2_level(0), _TVOC_level(0) {}

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

    const bool show_temperature_in_celcius() const {
        return _temp_C;
    }

    const float temp_C_to_F(const float temp) const {
        return temp*1.8 + 32;
    }

    //Update CO2 & TVOC levels
    void update_CO2_TVOC_levels(){
        int i;
        for(i = 0; i < 5; i++){
            if(spg30_co2 <_co2_levels[i]){
                _CO2_level = i;
                break;
            }
        }

        for(i = 0; i < 5; i++){
            if(spg30_tvoc <_tvoc_levels[i]){
                _TVOC_level = i;
                break;
            }
        }
        logger::log(logger::LLOG::DEBUG, "Ctxt", std::string(__func__) + "Levels CO2: " + std::to_string(_CO2_level) +
            " TVOC: " + std::to_string(_TVOC_level));
    }

    //
    const int get_CO2_level() const {
        return _CO2_level;
    }
    //
    const std::string& get_level_label(const int level) const {
        if(level<2) return get_str(StrID::Good);
        if(level==2) return get_str(StrID::Moderate);
        if(level==3) return get_str(StrID::Poor);
        return get_str(StrID::Dangerous);
    }

    //
    const int get_TVOC_level() const {
        return _TVOC_level;
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

    const bool is_lcd() const {
        return _lcd_on;
    }

    void set_lcd(const bool lcd_on_off){
        _lcd_on = lcd_on_off;
    }

private:
    LcdStrings _strs;

    //What should be used for temperature showing
    // true - celcius, false - fahrenheit
    bool _temp_C = true;

    //if LCD switched ON
    bool _lcd_on = true;

    //Concetration CO2 and/or TVOC
    int _CO2_level;
    int _TVOC_level;

    // Measurement interval for SPG30
    //  TVOC:  0 – 60'000 ppb
    //  CO2eq: 0 – 60'000 ppm
    //
    // ----- TVOC ----
    // Level        Hygienic                    TVOC [ppb]
    // 5  Unhealty  Situation not acceptable    2200 – 5500
    // 4  Poor      Major objections            660 – 2200
    // 3  Moderate  Some objections             220 – 660
    // 2  Good      No relevant objections      65 – 220
    // 1  Excellent No objections               0 – 65

    // ----- CO2 ----
    // Level
    // 1  250-350ppm	    Normal background concentration in outdoor ambient air
    // 2  350-1,000ppm	    Concentrations typical of occupied indoor spaces with good air exchange
    // 3  1,000-2,000ppm	Complaints of drowsiness and poor air.
    // 4  2,000-5,000 ppm	Headaches, sleepiness and stagnant, stale, stuffy air.
    //                      Poor concentration, loss of attention, increased heart rate and slight nausea may also be present.
    //    5,000	            Workplace exposure limit (as 8-hour TWA) in most jurisdictions.
    // 5  >40,000 ppm	    Exposure may lead to serious oxygen deprivation resulting in permanent brain damage, coma, even death.

    uint16_t _tvoc_levels[5] = {65, 220, 660, 2200, 60000};
    uint16_t _co2_levels[5] = {350, 1000, 2000, 5000, 60000};
};

}

#endif /* WEATHER_CONTEXT_H_ */
