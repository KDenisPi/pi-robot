/*
 * tsl2561.cpp
 * I2C Light-to-Digital Converter
 *  
 * 
 *  Note: This device can use three different addresses 0x29, 0x39 (default), 0x49 
 * 
 *  Created on: Mar 5, 2018
 *      Author: Denis Kudia
 */

#include "wiringPi.h"
#include "logger.h"
#include "I2CWrapper.h"
#include "tsl2561.h"

namespace pirobot {
namespace item {

const char TAG[] = "TSL2561";
const uint8_t Tsl2561::s_i2c_addr = 0x39;

//
//
//
Tsl2561::Tsl2561(const std::string& name, 
        const std::shared_ptr<pirobot::i2c::I2C> i2c,
        const uint8_t i2c_addr,
        const std::string& comment) :
    Item(name, comment, ItemTypes::SGP30), _i2caddr(i2c_addr) {

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));

    //register I2C user
    i2c->add_user(name, _i2caddr);

    I2CWrapper::lock();
    m_fd = I2CWrapper::I2CSetup(_i2caddr);
    I2CWrapper::unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));
}

//
//
Tsl2561::~Tsl2561(){

}

}
}