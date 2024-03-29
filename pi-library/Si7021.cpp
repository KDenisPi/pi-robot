/*
 * Si7021.cpp
 * I2C Humidity and Temperature Sensor
 *
 *  Created on: Feb 18, 2018
 *      Author: Denis Kudia
 */

#include <cmath>
#include "logger.h"
#include "I2C.h"
#include "Si7021.h"

namespace pirobot {
namespace item {

const char TAG[] = "Si7021";
const uint8_t Si7021::s_i2c_addr = 0x40;

const uint8_t Si7021::s_measure_RH[4] = {12, 8, 10, 11};
const uint8_t Si7021::s_measure_Temp[4] = {14,12,13,11};


Si7021::Si7021(const std::string& name, const std::shared_ptr<pirobot::i2c::I2C> i2c, const std::string& comment) :
    Item(name, comment, ItemTypes::SI7021), _i2c(i2c),_i2caddr(s_i2c_addr), _user_reg(0x00), values({0.0, 0.0}) {

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));

    //register I2C user
    m_fd = _i2c->add_user(name, _i2caddr);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));

    //load current parameters
    uint8_t user_reg = get_user_reg();
    uint8_t mes_res = get_measument_resolution();

    firmware();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Measument Resolution  Relative Humidity: " + std::to_string(s_measure_RH[mes_res]) +
            " bit. Temperature: " + std::to_string(s_measure_Temp[mes_res]) + " bit.");

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " VDD status: " + ( is_VDD_OK() ? "OK" : "Low"));
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " On-chip Heater: " + ( is_Heater_Enabled() ? "Enable" : "Disable"));

    //we do not need on heater now
    if(is_Heater_Enabled()){
        set_heater(false);
    }

    //wait 100ms before use
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

Si7021::~Si7021(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));
    _i2c->del_user(name(), m_fd);
}

//
//Read user register value
//
uint8_t Si7021::get_user_reg(){
    _user_reg = _i2c->I2CReadReg8(m_fd, SI7021_READ_UR_1);
    _stat_info.read(_user_reg);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Value: " + std::to_string(_user_reg));
    return _user_reg;
}

//
// Set user register value
//
void Si7021::set_user_reg(const uint8_t value){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Value: " + std::to_string(_user_reg));
    int status = _i2c->I2CWriteReg8(m_fd, SI7021_WRITE_UR_1, value);

    _stat_info.write(status);
}

//Reset
void Si7021::reset(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));
    int status = _i2c->I2CWrite(m_fd, SI7021_RESET);

    _stat_info.write(status);
    //wait 15ms before use
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//
//Read firmware value
//
void Si7021::firmware(){
    int status = _i2c->I2CWriteReg8(m_fd, SI7021_READ_FIRMWARE_1, SI7021_READ_FIRMWARE_2);
    _stat_info.read(status);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    uint8_t _firmware = _i2c->I2CRead(m_fd);

    _stat_info.read(_firmware);
    //possible values  0xFF - 1.0 , 0x20 - 2.0
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Firmaware version: " + (_firmware == 0xFF ? "1.0" : "2.0") + " RAW: " + std::to_string(_firmware));
}


//
// On/Off heater
//
void Si7021::set_heater(const bool enable){

    if( (enable && is_Heater_Enabled()) || (!enable && !is_Heater_Enabled()) ){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Nothing to do : Current: " + std::to_string(is_Heater_Enabled()) + " New: " + std::to_string(enable));
    }

    uint8_t user_reg = (enable ? (_user_reg | SI7021_UR_HTRE ) : (user_reg & (~SI7021_UR_HTRE)));
    set_user_reg(user_reg);
    get_user_reg();//temporal solution

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " On-chip Heater: " + ( is_Heater_Enabled() ? "Disable" : "Enable"));
}

//
//Do measument Measure Relative Humidity and Temperature
//
void Si7021::get_results(float& humidity, float& temperature, float& abs_humidity){
    uint8_t mrh[2] = {0,0}, temp[2] = {0,0};
    uint16_t last_MRH = 0, last_Temp = 0;

    // Measure Relative Humidity - send command
    int status = _i2c->I2CWrite(m_fd, SI7021_MRH_NHMM);
    if(status < 0)
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + "SI7021_MRH_NHMM error");

    _stat_info.write(status);
    //accordingly specification it takes us 12ms for MRH and 11ms for Temp = 30ms
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    //read two bytes
    mrh[1] = _i2c->I2CRead(m_fd);
    mrh[0] = _i2c->I2CRead(m_fd);
    last_MRH = mrh[1]*256 + mrh[0];

    //TGemperature already measured durint MRH detecting - just read value
    status = _i2c->I2CWrite(m_fd, SI7021_RT_NHMM);

    temp[1] = _i2c->I2CRead(m_fd);
    temp[0] = _i2c->I2CRead(m_fd);
    last_Temp = temp[1]*256 + temp[0];

    _stat_info.write(status);
    _stat_info.read(mrh[0]);
    _stat_info.read(mrh[1]);
    _stat_info.read(temp[0]);
    _stat_info.read(temp[1]);

    //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " MRH Full:" + std::to_string(last_MRH) +  " MS: " + std::to_string(mrh[1]) + " LS: " + std::to_string(mrh[0]));
    //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Temp Full:" + std::to_string(last_Temp) +  " MS: " + std::to_string(temp[1]) + " LS: " + std::to_string(temp[0]));

    values._last_MRH = (125*last_MRH)/65536 - 6;
    //check border values
    if(values._last_MRH < 0)
        values._last_MRH == 0.0;
    else if(values._last_MRH > 100.0)
        values._last_MRH = 100.0;

    values._last_Temp = (175.72*last_Temp)/65536 - 46.85;

    temperature = std::round(values._last_Temp*10)/10;
    humidity = std::round(values._last_MRH);
    abs_humidity = get_absolute_humidity(temperature, humidity);

    //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " MRH RAW: " + std::to_string(last_MRH) + " MRH: " + std::to_string(values._last_MRH));
    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Temperature : " + std::to_string(temperature) + " Humidity: " + std::to_string(humidity) + "%" + " Abs: " + std::to_string(abs_humidity));
}

}// item
}//pirobot
