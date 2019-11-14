/*
 * sgp30.cpp
 * I2C Sensurion Gas Platform
 *
 *  Note: This device always uses 0x58 address
 *
 *  Created on: Feb 21, 2018
 *      Author: Denis Kudia
 */
#include <cmath>

#include "wiringPi.h"
#include "logger.h"
#include "I2CWrapper.h"
#include "sgp30.h"
#include "crc.h"
#include "timers.h"

namespace pirobot {
namespace item {

const char TAG[] = "SGP30";
const uint8_t Sgp30::s_i2c_addr = 0x58;

//
//
//
Sgp30::Sgp30(const std::string& name, const std::shared_ptr<pirobot::i2c::I2C> i2c, const std::string& comment) :
    Item(name, comment, ItemTypes::SGP30), _i2caddr(s_i2c_addr), _humidity(0) {

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));

    values.uiCO2  = 0;
    values.uiTVOC = 0;
    baseline.uiCO2  = 0;
    baseline.uiTVOC = 0;

    //register I2C user
    i2c->add_user(name, _i2caddr);

    I2CWrapper::lock();
    m_fd = I2CWrapper::I2CSetup(_i2caddr);
    I2CWrapper::unlock();

    get_feature_set_version();

    //measure_test();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));
}


//Execute measure test, true - chip is OK
bool Sgp30::measure_test(){
    uint8_t data[3] = {0x00, 0x00, 0x00};

    read_data(data, 3, SGP30_MEASURE_TEST, 220);

    uint8_t crc_check = pirobot::crc::crc(data, 2, 0xFF, 0x31);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " 1:[" + std::to_string(data[0]) + "] 2:[" + std::to_string(data[1]) + "] 3:[" + std::to_string(data[2]) + "] CRC:[" + std::to_string(crc_check)+"]" );

    if(data[2] == crc_check && SGP30_MEASURE_TEST_PATTERN == convert_result(data)){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Success");
        return true;
    }

    return false;
}

//
// Read data
int Sgp30::read_data(uint8_t* data, const int len, const uint16_t cmd, const int delay_ms){
    int msb = (cmd >> 8);
    int lsb = (cmd & 0x00FF);

    //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " MSB: " + std::to_string(msb) + " LSB:" + std::to_string(lsb));

    I2CWrapper::lock();
    int status = I2CWrapper::I2CWriteReg8(m_fd, msb, lsb);
    _stat_info.write(status);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

    status = I2CWrapper::I2CReadData(m_fd, msb, data, len);
    _stat_info.read(status);

    I2CWrapper::unlock();

    //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Result Len: " + std::to_string(status));
    return status;
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
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    int msb = (SGP30_INIT_AIR_QUALITY >> 8);
    int lsb = (SGP30_INIT_AIR_QUALITY & 0x00FF);

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, msb, lsb);
    I2CWrapper::unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + "  Set baseline");
    _set_baseline();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + "  Set humidity");
    _set_humidity();
}

// Measure air quality
void Sgp30::measure_air_quality(){
    uint8_t data[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t crc_check[2] = {0x00, 0x00};
    uint16_t uiCO2;
    uint16_t uiTVOC;
    bool co2_success, tvoc_success;

    read_data(data, 6, SGP30_MEASURE_AIR_QUALITY, 10);

    crc_check[0] = pirobot::crc::crc(&data[0], 2, 0xFF, 0x31);
    crc_check[1] = pirobot::crc::crc(&data[3], 2, 0xFF, 0x31);

    co2_success = (crc_check[0] == data[2]);
    tvoc_success = (crc_check[1] == data[5]);

    uiCO2  = convert_result(data);
    uiTVOC = convert_result(&data[3]);

    // CO2 == 400 & TVOC == 0 are special values during initialization
    if((uiCO2 == 400) && (uiTVOC == 0))
        return;

    std::lock_guard<std::mutex> lk(cv_m_data);
    if(co2_success){
        values.uiCO2  = uiCO2;
        //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " CO2: " + std::to_string(values.uiCO2));
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " CO2 CRC Invalid");

    if(tvoc_success){
        values.uiTVOC = uiTVOC;
        //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " TVOC: " + std::to_string(values.uiTVOC));
    }
    else
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " TVOC CRC Invalid");
}

// Get baseline
void Sgp30::get_baseline(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

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

    //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Get Baseline CO2: " + std::to_string(base_co2) + " TVOC: " + std::to_string(base_tvoc));

    std::lock_guard<std::mutex> lk(cv_m_data);
    baseline.uiCO2 = base_co2;
    baseline.uiTVOC = base_tvoc;
}

//Write data
int Sgp30::write_data(uint8_t* data, const int len, const uint16_t cmd, const int delay_ms){
    int msb = (cmd >> 8);
    int lsb = (cmd & 0x00FF);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " MSB: " + std::to_string(msb) + " LSB:" + std::to_string(lsb));

    I2CWrapper::lock();
    int status = I2CWrapper::I2CWriteData(m_fd, msb, data, len);
    I2CWrapper::unlock();

    _stat_info.write(status);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Result Data Len: " + std::to_string(status));

    return status;
}

/*
Set humidity

The 2 data bytes represent humidity values as a fixed-point
8.8bit number with a minimum value of 0x0001 (=1/256 g/m3 ) and a maximum value of 0xFFFF (255 g/m3 + 255/256 g/m3 ).
For instance, sending a value of 0x0F80 corresponds to a humidity value of 15.50 g/m3 (15 g/m3 + 128/256 g/m3 ).
*/
void Sgp30::set_humidity(const float humidity){

    uint16_t h_high = (uint16_t)std::trunc(humidity);
    uint16_t h_low =  (uint16_t)(256 * (humidity - std::trunc(humidity)));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Humidity: " + std::to_string(humidity) + " High:" + std::to_string(h_high) + " Low:" + std::to_string(h_low));

    uint16_t h_high_now = (_humidity >> 8);
    uint16_t h_low_now =  (_humidity & 0x00FF);

    /*
    * Check humidity changes
    * Low range between [1-256]
    */
    if(h_high_now == h_high){
        uint16_t diff = (h_low_now > h_low ? (h_low_now-h_low) : (h_low - h_low_now));

        //do not update if difference less than 5% on low part
        if( diff < 12){ //5%
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Changes are too small. Current low: " + std::to_string(h_low_now));
            return;
        }
    }

    std::lock_guard<std::mutex> lk(cv_m_data);
    _humidity = (h_high << 8) | h_low;
}

//Set humidity
void Sgp30::_set_humidity(){
    uint16_t hmd;

    {
        std::lock_guard<std::mutex> lk(cv_m_data);
        hmd = _humidity;
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Humidity: " + std::to_string(hmd));
    if(hmd == 0){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Humidity have not set. Do nothing.");
        return;
    }

    uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};

    int msb = (SGP30_SET_HUMIDITY >> 8);
    int lsb = (SGP30_SET_HUMIDITY & 0x00FF);

    data[0] = lsb;
    data[1] = (hmd >> 8);
    data[2] = (hmd & 0x00FF);
    data[3] = pirobot::crc::crc(&data[1], 2, 0xFF, 0x31);

    write_data(data, 4, SGP30_SET_HUMIDITY, 10);
}

// NOTE: Set baseline use parameter TVOC, CO2 (other functions CO2, TVOC)
void Sgp30::set_baseline(const uint16_t base_co2, const uint16_t base_tvoc){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Baseline CO2: " + std::to_string(base_co2) + " TVOC: " + std::to_string(base_tvoc));

    std::lock_guard<std::mutex> lk(cv_m_data);
    baseline.set(base_co2, base_tvoc);
}

//Set baseline
// NOTE: Set baseline use parameter TVOC, CO2 (other functions CO2, TVOC)
void Sgp30::_set_baseline(){
    Sgp30_measure bln;

    {
        std::lock_guard<std::mutex> lk(cv_m_data);
        bln = baseline;
    }
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Baseline CO2: " + std::to_string(bln.uiCO2) + " TVOC: " + std::to_string(bln.uiTVOC));

    uint8_t data[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    int msb = (SGP30_SET_BASELINE >> 8);
    int lsb = (SGP30_SET_BASELINE & 0x00FF);

    data[0] = lsb;

    data[4] = (bln.uiCO2 >> 8);
    data[5] = (bln.uiCO2 & 0x00FF);
    data[6] = pirobot::crc::crc(&data[4], 2, 0xFF, 0x31);

    data[1] = (bln.uiTVOC >> 8);
    data[2] = (bln.uiTVOC & 0x00FF);
    data[3] = pirobot::crc::crc(&data[1], 2, 0xFF, 0x31);

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

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " init_air_quality. Name: " + name);
    owner->init_air_quality();

    uint16_t hmdt = owner->_get_humidity();

    while(!owner->is_stop_signal()){
        long ms_start = piutils::timers::Timers::milliseconds();
        owner->measure_air_quality();

#ifdef SGP30_DEBUG
        owner->ddata_put(ms_start);
#endif
        uint16_t hmdt_now = owner->_get_humidity();
        if(hmdt_now != hmdt){
            owner->_set_humidity();
            hmdt = hmdt_now;
        }

        long slp_time = (1000 - (piutils::timers::Timers::milliseconds() - ms_start));
        std::this_thread::sleep_for(std::chrono::milliseconds(slp_time)); //1sec - 1Hz
    }

    //save baseline values at the end of measure cycle
    owner->get_baseline();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished. Name: " + name);
}

//Get measure results
void Sgp30::get_results(uint16_t& co2, uint16_t& tvoc){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));

    {
        std::lock_guard<std::mutex> lk(cv_m_data);
        co2 = values.uiCO2;
        tvoc = values.uiTVOC;
    }

    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " CO2: " + std::to_string(co2) + " TVOC: " + std::to_string(tvoc));
}

//Get measure results
void Sgp30::get_baseline(uint16_t& bs_co2, uint16_t& bs_tvoc){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));

    {
        std::lock_guard<std::mutex> lk(cv_m_data);
        bs_co2 = baseline.uiCO2;
        bs_tvoc = baseline.uiTVOC;
    }

    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Base: CO2: " + std::to_string(bs_co2) + " TVOC: " + std::to_string(bs_tvoc));
}


}//item
}//pirobot
