/*
 * Si7021.cpp
 * I2C Humidity and Temperature Sensor
 *
 *  Created on: Feb 18, 2018
 *      Author: Denis Kudia
 */

#include "logger.h"
#include "I2C.h"
#include "I2CWrapper.h"
#include "Si7021.h"

namespace pirobot {
namespace item {

const char TAG[] = "MCP23017";
const uint8_t Si7021::s_i2c_addr = 0x40;

const uint8_t Si7021::s_measure_RH[4] = {12, 8, 10, 11};
const uint8_t Si7021::s_measure_Temp[4] = {14,12,13,11};


Si7021::Si7021(const std::string& name, const std::shared_ptr<pirobot::i2c::I2C> i2c, const std::string& comment) : 
    Item(name, comment, ItemTypes::SI7021), _i2caddr(s_i2c_addr), _user_reg(0x00) {

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));

    //register I2C user
    i2c->add_user(name, _i2caddr);

    I2CWrapper::lock();
    m_fd = I2CWrapper::I2CSetup(_i2caddr);
    I2CWrapper::unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));

    //load current parameters
    uint8_t user_reg = get_user_reg();
    uint8_t mes_res = get_measument_resolution();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Measument Resolution  Relative Humidity: " + std::to_string(s_measure_RH[mes_res]) + 
            " bit. Temperature: " + std::to_string(s_measure_Temp[mes_res]) + " bit.");

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " VDD status: " + ( is_VDD_OK() ? "OK" : "Low"));
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " On-chip Heater: " + ( is_Heater_Enabled() ? "Enable" : "Disable"));

    //we do not need on heater now
    if(is_Heater_Enabled()){
        set_heater(false);
    }
}

Si7021::~Si7021(){

}

uint8_t Si7021::get_user_reg(){
    I2CWrapper::lock();
    I2CWrapper::I2CWrite(m_fd, SI7021_READ_UR_1);
    _user_reg = (uint8_t)I2CWrapper::I2CRead(m_fd);
    I2CWrapper::unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Value: " + std::to_string(_user_reg));
    return _user_reg;
}

void Si7021::set_user_reg(const uint8_t value){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Value: " + std::to_string(_user_reg));

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, SI7021_WRITE_UR_1, value);
    I2CWrapper::unlock();
}


void Si7021::set_heater(const bool enable){
    if( (enable && is_Heater_Enabled()) || (!enable && !is_Heater_Enabled()) ){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Nothing to do : Current: " + std::to_string(is_Heater_Enabled()) + " New: " + std::to_string(enable));
    }

    uint8_t user_reg = (enable ? (_user_reg | SI7021_UR_HTRE ) : (user_reg & (~SI7021_UR_HTRE)));
    set_user_reg(user_reg);

    get_user_reg();//temporal solution

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " On-chip Heater: " + ( is_Heater_Enabled() ? "Disable" : "Enable"));

}


}// item
}//pirobot
