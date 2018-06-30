/*
 * measurement.h
 *
 *  Created on: June 21, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_MEASUREMENT_H_
#define WEATHER_MEASUREMENT_H_

#include <stdint.h>
#include <ctime>

#include "smallthings.h"

namespace weather {

class Measurement {
public:
    Measurement() {
        // Si7021 - I2C Humidity and Temperature Sensor
        si7021_humidity = 0.0;
        si7021_temperature = 0.0;
        si7021_abs_humidity = 0.0;

        // SPG30 - I2C Sensurion Gas Platform
        spg30_co2 = 0;
        spg30_tvoc = 0;
        spg30_base_co2 = 0;
        spg30_base_tvoc = 0;

        // BMP280 - I2C Digital Presure Sensor
        bmp280_pressure = 0.0;
        bmp280_temperature = 0.0;
        bmp280_altitude = 0.0;

        // TSL2561 - I2C Light-to-Digital Converter
        tsl2651_lux = 0;
    }

    virtual ~Measurement() {}

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

    Measurement& operator=(const Measurement& m){
        this->si7021_humidity = m.si7021_humidity;
        this->si7021_temperature = m.si7021_temperature;
        this->si7021_abs_humidity = m.si7021_abs_humidity;

        // SPG30 - I2C Sensurion Gas Platform
        this->spg30_co2 = m.spg30_co2;
        this->spg30_tvoc = m.spg30_tvoc;
        this->spg30_base_co2 = m.spg30_base_co2;
        this->spg30_base_tvoc = m.spg30_base_tvoc;

        // BMP280 - I2C Digital Presure Sensor
        this->bmp280_pressure = m.bmp280_pressure;
        this->bmp280_temperature = m.bmp280_temperature;
        this->bmp280_altitude = m.bmp280_altitude;

        // TSL2561 - I2C Light-to-Digital Converter
        this->tsl2651_lux = m.tsl2651_lux;
    }

    const float temp_C_to_F(const float temp) const {
        return temp*1.8 + 32;
    }

    //
    // Return temperature
    // Due to I have some differenced between Si7021 & Bmp280 I make decision return middle value
    //
    const float temp(const bool celcius = true){
        float tempr = (si7021_temperature + bmp280_temperature)/2;
        return (celcius ? tempr : temp_C_to_F(tempr));
    }

    const char* to_string(){
        std::tm tm;
        piutils::get_time(tm);


        //
        // Output format:
        // 1. Date&time YYYY/MM/DD HH:MM:SS
        // 2. Humidity (0-100%)
        // 3. Temperature (C or F, 3-digits, signed)
        // 4. Pessure (mm Hg, 3-digits)
        // 5. Luximity (Lux, 0-40000, 5-digits)
        // 6. CO2 (0 – 60'000 ppm, 5-digits)
        // 7. TVOC (0 – 60'000 ppb, 5-digits)
        // 8. Altitude (0-5000, 4-digits)
        //
        sprintf(_buff, "%d/%d/%d %02d:%02d:%02d,%.0f,%.0f,%.0f,%d,%d,%d,%.0f\n",
            1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
            si7021_humidity,
            temp(),
            bmp280_pressure,
            tsl2651_lux,
            spg30_co2,
            spg30_tvoc,
            bmp280_altitude);

        return _buff;
    }

    static const int buff_size = 128;
private:
    char _buff[buff_size];
};

}//namespace weather
#endif