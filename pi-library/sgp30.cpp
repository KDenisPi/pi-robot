/*
 * sgp30.cpp
 * I2C Sensurion Gas Platform
 *
 *  Note: This device always uses 0x58 address
 * 
 *  Created on: Feb 21, 2018
 *      Author: Denis Kudia
 */

#include "logger.h"
#include "I2CWrapper.h"
#include "sgp30.h"

namespace pirobot {
namespace item {

const char TAG[] = "SGP30";
const uint8_t Sgp30::s_i2c_addr = 0x58;

//
//
//
Sgp30::Sgp30(const std::string& name, const std::shared_ptr<pirobot::i2c::I2C> i2c, const std::string& comment) : 
    Item(name, comment, ItemTypes::SI7021), _i2caddr(s_i2c_addr) {

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));

    //register I2C user
    i2c->add_user(name, _i2caddr);

    I2CWrapper::lock();
    m_fd = I2CWrapper::I2CSetup(_i2caddr);
    I2CWrapper::unlock();

    get_feature_set_version();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));
}

//
// Get feature set version. Data return 3 bytes. Delay max 2ms 
//
void Sgp30::get_feature_set_version(){
    uint8_t data[2] = {0x00, 0x00};
    uint8_t crc = 0x00;

    I2CWrapper::lock();

    I2CWrapper::I2CWriteReg8(m_fd, SGP30_GET_FEATURE_SET_VERSION_1, SGP30_GET_FEATURE_SET_VERSION_2);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    data[0] = I2CWrapper::I2CRead(m_fd);
    data[1] = I2CWrapper::I2CRead(m_fd);
    crc = I2CWrapper::I2CRead(m_fd);

    I2CWrapper::unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Feature set RAW MSB: " + std::to_string(data[0]) + " LSB:" + std::to_string(data[1]) + " CRC:" + std::to_string(crc));
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Product Type: " + std::to_string(data[0] >> 5) + " Version: " + std::to_string(data[1]));
}

//
//
//
Sgp30::~Sgp30(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));

    stop();
}

//
//
//
void Sgp30::stop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));
    piutils::Threaded::stop();
}

/*
 * Accordingly specification on-chip baseline compensation algorithm has been optimized for 1Hz sampling rate.
 *
 */
 void Sgp30::worker(Sgp30* owner){
    std::string name = owner->name();
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker started. Name: " + name);

    while(!owner->is_stop_signal()){

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));    
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished. Name: " + name);
}

}//item
}//pirobot
