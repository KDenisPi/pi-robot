/*
 * sgp30.cpp
 * I2C Sensurion Gas Platform
 *
 *  Note: This device always uses 0x58 address
 * 
 *  Created on: Feb 21, 2018
 *      Author: Denis Kudia
 */

#include "wiringPi.h"
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

    measure_test();
    
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));
}


//Execute measure test, true - chip is OK
bool Sgp30::measure_test(){
    uint8_t data[3] = {0x00, 0x00, 0x00};

    read_data(data, 3, SGP30_MEASURE_TEST, 220);

    uint8_t crc_check = pirobot::crc::crc(data, 2, 0xFF, 0x31);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " 1:[" + std::to_string(data[0]) + "] 2:[" + std::to_string(data[1]) + "] 3:[" + std::to_string(data[2]) + "] CRC:[" + std::to_string(crc_check)+"]" );

    if(data[2] == crc_check && SGP30_MEASURE_TEST_PATTERN == convert_result(data)){
        return true;
    }

    return false;
}

//
// Read data 
int Sgp30::read_data(uint8_t* data, const int len, const uint16_t cmd, const int delay_ms){
    int msb = (cmd >> 8);
    int lsb = (cmd & 0x00FF);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " MSB: " + std::to_string(msb) + " LSB:" + std::to_string(lsb));

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, msb, lsb);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

    int res_len = I2CWrapper::I2CReadData(m_fd, msb, data, 3);
    I2CWrapper::unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Result Len: " + std::to_string(res_len));
    return res_len;
}

//
// Get feature set version. Data return 3 bytes. Delay max 2ms 
//
void Sgp30::get_feature_set_version(){
    uint8_t data[3] = {0x00, 0x00, 0x00};

    read_data(data, 3, SGP30_GET_FEATURE_SET_VERSION, 10);

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
    int msb = (SGP30_INIT_AIR_QUALITY >> 8);
    int lsb = (SGP30_INIT_AIR_QUALITY & 0x00FF);

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, msb, lsb);
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
        measure[0] = convert_result(data);
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " CO2 CRC Invalid");

    if(crc_check[1] == data[5]){
        measure[1] = convert_result(&data[3]);
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " TVOC CRC Invalid");

    if((crc_check[0] == data[2]) || (crc_check[1] == data[5])){
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

    if(crc_check[0] == data[2]){
        base_co2 = convert_result(data);
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " CO2 CRC Invalid");

    if(crc_check[1] == data[5]){
        base_tvoc = convert_result(&data[3]);
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " TVOC CRC Invalid");
    
    std::lock_guard<std::mutex> lk(cv_m);
    baseline.uiCO2 = base_co2;
    baseline.uiTVOC = base_tvoc;
}

int Sgp30::write_data(uint8_t* data, const int len, const uint16_t cmd, const int delay_ms){
    int msb = (cmd >> 8);
    int lsb = (cmd & 0x00FF);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " MSB: " + std::to_string(msb) + " LSB:" + std::to_string(lsb));

    I2CWrapper::lock();
    int res_len = I2CWrapper::I2CWriteData(m_fd, msb, data, len);
    I2CWrapper::unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Result Len: " + std::to_string(res_len));

    return res_len;
}

//Set humidity
void Sgp30::set_humidity(const uint16_t humidity){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Humidity: " + std::to_string(humidity));
    uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};

    int msb = (SGP30_SET_HUMIDITY >> 8);
    int lsb = (SGP30_SET_HUMIDITY & 0x00FF);

    data[0] = lsb;
    data[1] = (humidity >> 8);
    data[2] = (humidity & 0x00FF);
    data[3] = pirobot::crc::crc(&data[1], 2, 0xFF, 0x31);

    write_data(data, 4, SGP30_SET_HUMIDITY, 10);
}

//Set humidity
void Sgp30::set_baseline(const uint16_t base_c02, const uint16_t base_tvoc){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Baseline CO2: " + std::to_string(base_c02) + " TVOC: " + std::to_string(base_tvoc));
    uint8_t data[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int msb = (SGP30_SET_BASELINE >> 8);
    int lsb = (SGP30_SET_BASELINE & 0x00FF);

    data[0] = lsb;

    data[1] = (base_c02 >> 8);
    data[2] = (base_c02 & 0x00FF);
    data[3] = pirobot::crc::crc(&data[1], 2, 0xFF, 0x31);

    data[4] = (base_tvoc >> 8);
    data[5] = (base_tvoc & 0x00FF);
    data[6] = pirobot::crc::crc(&data[4], 2, 0xFF, 0x31);

    write_data(data, 7, SGP30_SET_BASELINE, 10);
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
