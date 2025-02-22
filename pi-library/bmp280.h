/*
 * bmp280.h
 * I2C Digital Presure Sensor
 *
 *  Note: This device uses address 0x76 if SDO connected to GND
 *  or 0x77 if SDO connected to VDD (3,3 V)
 *
 *  Created on: Mar 3, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_BMP280_H
#define PI_LIBRARY_BMP280_H

#include "item.h"
#include "I2C.h"

namespace pirobot {
namespace item {

#define BMP280_ID               0xD0
#define BMP280_RESET            0xE0
#define BMP280_STATUS           0xF3
#define BMP280_CTRL_MEASURE     0xF4
#define BMP280_CONFIG           0xF5
#define BMP280_PRESSURE_MSB     0xF7
#define BMP280_PRESSURE_LSB     0xF8
#define BMP280_PRESSURE_XLSB    0xF9
#define BMP280_TEMP_MSB         0xFA
#define BMP280_TEMP_LSB         0xFB
#define BMP280_TEMP_XLSB        0xFC

#define BMP280_ID_CHIP_NUMBER   0x58 //this value will be return from ID register as soon as power on reset finished
#define BMP280_RESET_CODE       0xB6 //command to start power on rest procedure

#define BMP280_STATUS_MEASURING     0x04
#define BMP280_STATUS_DATA_UPDATE   0x01

#define BMP280_OVERSAMPLING_SKIPPED    0x00
#define BMP280_OVERSAMPLING_1          0x01
#define BMP280_OVERSAMPLING_2          0x02
#define BMP280_OVERSAMPLING_4          0x03
#define BMP280_OVERSAMPLING_8          0x04
#define BMP280_OVERSAMPLING_16         0x05

#define BMP280_POWER_MODE_SLEEP         0x00
#define BMP280_POWER_MODE_FORCED        0x01 // or 0x02
#define BMP280_POWER_MODE_NORMAL        0x03

#define BMP280_SPI_OFF      0x00
#define BMP280_SPI_ON       0x01

#define BMP280_FILTER_OFF   0x00
#define BMP280_FILTER_2     0x02
#define BMP280_FILTER_4     0x04
#define BMP280_FILTER_8     0x08
#define BMP280_FILTER_16    0x0F

//registers for compensation values
#define BMP280_dig_T1   0x88
#define BMP280_dig_T2   0x8A
#define BMP280_dig_T3   0x8C

#define BMP280_dig_P1   0x8E
#define BMP280_dig_P2   0x90
#define BMP280_dig_P3   0x92
#define BMP280_dig_P4   0x94
#define BMP280_dig_P5   0x96
#define BMP280_dig_P6   0x98
#define BMP280_dig_P7   0x9A
#define BMP280_dig_P8   0x9C
#define BMP280_dig_P9   0x9E

class Bmp280 : public item::Item {

public:
    Bmp280(const std::string& name, const std::shared_ptr<pirobot::i2c::I2C> i2c,
            const uint8_t i2c_addr = s_i2c_addr,
            const uint8_t mode = BMP280_POWER_MODE_FORCED,
            const uint8_t pressure_oversampling = 1,
            const uint8_t temperature_oversampling = 0xFF,
            const uint8_t standby_time = 0x05,
            const uint8_t filter = BMP280_FILTER_OFF,
            const int spi = 0,
            const int spi_channel = 0,
            const std::string& comment = "");

    virtual ~Bmp280();

    static const uint8_t s_i2c_addr;
    static const char* s_measure_mode[4];
    static const char* s_oversampling[8];
    static const char* s_standby_time[8];

    //Detect mode by name.
    //TODO: exception for unknown name?
    static const uint8_t get_mode_by_name(const std::string& mode){
        if(mode == "FORCED") return BMP280_POWER_MODE_FORCED;
        if( mode == "NORMAL") return BMP280_POWER_MODE_NORMAL;

        return BMP280_POWER_MODE_SLEEP;
    }

    virtual const std::string printConfig() override{
        return std::string(" BMP280 I2C: ") + std::to_string(_i2caddr);
    }

    //Reset
    void reset();

    //Get chip ID
    uint8_t get_id();

    //Get status
    const uint8_t get_status();

    //Get measure control parameters
    const uint8_t get_measure_control();

    //Get measure mode
    const uint8_t get_measure_mode(const uint8_t meas_ctrl){
        uint8_t mode = (meas_ctrl & 0x03);
        if( mode == 1 || mode == 2)
            return BMP280_POWER_MODE_FORCED;

        return mode;
    }

    //Get config
    const uint8_t get_config();

    //Return current pressure, temperature, altitude values
    const bool get_results(float& pressure, float& temp, float& altitude);

    //
    //Set measure control parameters
    //
    // -- Type --                   -- Pressure --                      -- Temp --
    // Pressure measurement off     BMP280_OVERSAMPLING_SKIPPED         Any valid value
    // Ultra low power              BMP280_OVERSAMPLING_1               BMP280_OVERSAMPLING_1
    // Low power                    BMP280_OVERSAMPLING_1               BMP280_OVERSAMPLING_1
    // Standart resolution          BMP280_OVERSAMPLING_1               BMP280_OVERSAMPLING_1
    // High resolution              BMP280_OVERSAMPLING_1               BMP280_OVERSAMPLING_1
    // Ultra High resolution        BMP280_OVERSAMPLING_1               BMP280_OVERSAMPLING_2
    //
    // Note:  Temperature oversampling set by pressure oversampling if not set
    bool set_measure_control(const uint8_t power_mode, const uint8_t pressure_over, const uint8_t temp_over = 0xFF);

    //Set config
    bool set_config(const uint8_t spi, const uint8_t filter, const uint8_t standby_time);

private:
    uint8_t _i2caddr;
    int m_fd;
    std::shared_ptr<pirobot::i2c::I2C> _i2c;

    uint8_t m_mode;
    uint8_t m_pressure_oversampling;
    uint8_t m_temperature_oversampling;
    uint8_t m_standby_time; //used for NORMAL mode only, 0-8
    uint8_t m_filter; //0,2,4,8,16
    int m_spi;
    int m_spi_channel;

    //compensation values
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    int32_t t_fine;

    void read_compensation();

    float calculate_Temperature(int32_t temperature);
    float calculate_Pressure(int32_t pressure);

    float read_Altitude(const float pressure, const float seaLevelhPa = 1013.25);

    //construct value
    const int32_t construct_value(const uint32_t msb, const uint32_t lsb, const uint32_t xlsb){
        return (int32_t) ((msb << 12) | (lsb << 4) | xlsb);
    }
};

}//item
}//pirobot

#endif