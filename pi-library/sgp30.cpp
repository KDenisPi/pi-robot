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
#include "crc.h"

namespace pirobot {
namespace item {

const char TAG[] = "SGP30";
const uint8_t Sgp30::s_i2c_addr = 0x58;

//
//
//
Sgp30::Sgp30(const std::string& name, const std::shared_ptr<pirobot::i2c::I2C> i2c, const std::string& comment) : 
    Item(name, comment, ItemTypes::SGP30), _i2caddr(s_i2c_addr) {

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
// Read data 
void Sgp30::read_data(uint8_t* data, const int len, const uint16_t cmd, const int delay){

    I2CWrapper::lock();
    I2CWrapper::I2CWrite(m_fd, (cmd >> 8));
    I2CWrapper::I2CWrite(m_fd, (cmd & 0x00FF));
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    for(int i = 0; i < len; i++){
        data[i] = I2CWrapper::I2CRead(m_fd);
    }

    I2CWrapper::unlock();
}

//
// Get feature set version. Data return 3 bytes. Delay max 2ms 
//
void Sgp30::get_feature_set_version(){
    uint8_t data[3] = {0x00, 0x00, 0x00};

    read_data(data, 3, SGP30_GET_FEATURE_SET_VERSION, 2);

    //calculate CRC
    uint8_t crc_check = pirobot::crc::crc(data, 2, 0xFF, 0x31);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Feature set RAW MSB: " + std::to_string(data[0]) + " LSB:" + std::to_string(data[1]) + 
        " CRC: " + (data[2]==crc_check ? "OK " : "Invalid ") + std::to_string(data[2]) );
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
// Init air quality
//
void Sgp30::init_air_quality(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));

    I2CWrapper::lock();
    I2CWrapper::I2CWrite(m_fd, (SGP30_INIT_AIR_QUALITY >> 8));
    I2CWrapper::I2CWrite(m_fd, (SGP30_INIT_AIR_QUALITY & 0x00FF));
    I2CWrapper::unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// Measure air quality
void Sgp30::measure_air_quality(){
    uint8_t data[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t crc_check[2] = {0x00, 0x00};
    uint16_t measure[2];

    get_results(measure);

    read_data(data, 6, SGP30_MEASURE_AIR_QUALITY, 10);

    crc_check[0] = pirobot::crc::crc(&data[0], 2, 0xFF, 0x31);
    crc_check[1] = pirobot::crc::crc(&data[3], 2, 0xFF, 0x31);

    if(crc_check[0] == data[2]){
        measure[0] = data[0];
        measure[0] = (measure[0] << 8 ) | data[1];
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " CO2 CRC Invalid");

    if(crc_check[1] == data[5]){
        measure[1] = data[3];
        measure[1] = (measure[1] << 8 ) | data[4];
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " TVOC CRC Invalid");

    if((crc_check[0] == data[2]) | (crc_check[1] == data[5])){
        put_results(measure[0], measure[1]);
    }
}

// Get baseline
void Sgp30::get_baseline(){
    uint8_t data[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t crc_check[2] = {0x00, 0x00};
    uint16_t base_co2 = baseline.uiCO2, base_tvoc = baseline.uiTVOC;

    read_data(data, 6, SGP30_GET_BASELINE, 10);

    crc_check[0] = pirobot::crc::crc(&data[0], 2, 0xFF, 0x31);
    crc_check[1] = pirobot::crc::crc(&data[3], 2, 0xFF, 0x31);

    if(crc_check[0] != data[2])
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " CO2 CRC Invalid");
    if(crc_check[1] != data[5])
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " TVOC CRC Invalid");

    if(crc_check[0] == data[2]){
        base_co2 = data[0];
        base_co2 = (base_co2 << 8 ) | data[1];
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " CO2 CRC Invalid");

    if(crc_check[1] == data[5]){
        base_tvoc = data[3];
        base_tvoc = (base_tvoc << 8 ) | data[4];
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " TVOC CRC Invalid");
    
    std::lock_guard<std::mutex> lk(cv_m);
    baseline.uiCO2 = base_co2;
    baseline.uiTVOC = base_tvoc;
}

//Set humidity
void Sgp30::set_humidity(const uint16_t humidity){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Humidity: " + std::to_string(humidity));

    uint8_t data[3] = {0x00, 0x00, 0x00};
    data[0] = (humidity >> 8);
    data[1] = (humidity & 0x00FF);
    data[2] = pirobot::crc::crc(&data[0], 2, 0xFF, 0x31);

    I2CWrapper::lock();
    I2CWrapper::I2CWrite(m_fd, (SGP30_SET_HUMIDITY >> 8));
    I2CWrapper::I2CWrite(m_fd, (SGP30_SET_HUMIDITY & 0x00FF));
    I2CWrapper::I2CWrite(m_fd, data[0]);
    I2CWrapper::I2CWrite(m_fd, data[1]);
    I2CWrapper::I2CWrite(m_fd, data[2]);
    I2CWrapper::unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
 */
 void Sgp30::worker(Sgp30* owner){
    std::string name = owner->name();
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker started. Name: " + name);

    owner->init_air_quality();

    while(!owner->is_stop_signal()){

        owner->measure_air_quality();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //1sec - 1Hz
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished. Name: " + name);
}

}//item
}//pirobot
