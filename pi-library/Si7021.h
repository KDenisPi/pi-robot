/*
 * Si7021.h
 * I2C Humidity and Temperature Sensor
 *
 *  Note: This device always uses 0x40 address
 *
 *  Created on: Feb 18, 2018
 *      Author: Denis Kudia
 */
#ifndef PI_LIBRARY_SI7021_H
#define PI_LIBRARY_SI7021_H

#include "item.h"
#include "I2C.h"
#include "statistics.h"

namespace pirobot {
namespace item {

#define SI7021_MRH_HMM      0xE5  // Measure Relative Humidity. Hold Master Mode
#define SI7021_MRH_NHMM     0xF5  // Measure Relative Humidity. No Hold Master Mode
#define SI7021_MT_HMM       0xE3  // Measure Temperature. Hold Master Mode
#define SI7021_MT_NHMM      0xF3  // Measure Temperature. No Hold Master Mode
#define SI7021_RT_NHMM      0xE0  // Read Temperatur Value from Previous RH Measurement
#define SI7021_RESET        0xFE  // Reset
#define SI7021_WRITE_UR_1   0xE6  // Write RH/T User Register 1
#define SI7021_READ_UR_1    0xE7  // Read RH/T User Register 1
#define SI7021_WRITE_HC     0x51  // Write Heater Control Register
#define SI7021_READ_HC      0x11  // Read Heater Control Register
#define SI7021_READ_ID_1_1  0xFA
#define SI7021_READ_ID_1_2  0x0F
#define SI7021_READ_ID_2_1  0xFC
#define SI7021_READ_ID_2_2  0xC9
#define SI7021_READ_FIRMWARE_1    0x84
#define SI7021_READ_FIRMWARE_2    0xB8

// User Register
//
//Measurement Resolution
//      RH(bit) Temp(bit)
// 00   12      14
// 01   8       12
// 10   10      13
// 11   11      11
#define SI7021_UR_MR_HIGH   0x80 // Measurement Resolution High
#define SI7021_UR_MR_LOW    0x01 // Measurement Resolution Low

#define SI7021_UR_VDD_SATUS 0x40 // VDD Status (0-OK, 1-Low)
#define SI7021_UR_HTRE      0x04 // On chip Heater (1-Enable, 0-Disable)

class Si7021 :  public Item {

public:
    Si7021(const std::string& name, const std::shared_ptr<pirobot::i2c::I2C> i2c, const std::string& comment = "");
    virtual ~Si7021();

    static const uint8_t s_i2c_addr;
    static const uint8_t s_measure_RH[4];
    static const uint8_t s_measure_Temp[4];

    virtual const std::string printConfig() override{
        return std::string(" User Register. Measurement Resolution: ") + std::to_string(get_measument_resolution()) + " VDD OK: " + std::to_string(is_VDD_OK()) +
            " Heater: " + std::to_string(is_Heater_Enabled());
    }



private:
    uint8_t _i2caddr;
    int m_fd;
    std::shared_ptr<pirobot::i2c::I2C> _i2c;

    struct Si7021_data {
        float _last_MRH; //last value for MRH measument
        float _last_Temp;//last value for Temperature measument
    } values;

    uint8_t _user_reg; //current state of user register

    inline uint8_t get_user_reg();
    inline void set_user_reg(const uint8_t value);

    //I do not want to allow provide possibility to enable heater for now
    void set_heater(const bool enable);
    //print firmware version
    void firmware();
    //calculate absolute humidity
    float get_absolute_humidity(const float temperature, const float humidity);

    pirobot::stat::Statistics _stat_info;

public:
    //Detect Measurement Resolution from User Register value
    const uint8_t get_measument_resolution() const{
        uint8_t mes_res = 0x00; //Measurement Resolution
        if((_user_reg & SI7021_UR_MR_HIGH) != 0 ) mes_res |= 0x02;
        if((_user_reg & SI7021_UR_MR_LOW) != 0 ) mes_res |= 0x01;
        return mes_res;
    }

    const bool is_VDD_OK() const{
        return ((_user_reg & SI7021_UR_VDD_SATUS) == 0);
    }

    const bool is_Heater_Enabled(){
        return ((_user_reg & SI7021_UR_HTRE) != 0);
    }

    //make measurement
    void get_results(float& humidity, float& temperature, float& abs_humidity);

    //Reset
    inline void reset();
};

}//item
}//pirobot


#endif
