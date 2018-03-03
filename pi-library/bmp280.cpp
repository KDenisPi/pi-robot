/*
 * bmp280.cpp
 * I2C Digital Presure Sensor
 *
 *  Note: This device uses address 0x76 if SDO connected to GND
 *  or 0x77 if SDO connected to VDD (3,3 V)
 * 
 *  Created on: Mar 3, 2018
 *      Author: Denis Kudia
 */
#include "wiringPi.h"
#include "logger.h"
#include "I2CWrapper.h"
#include "bmp280.h"

namespace pirobot {
namespace item {

const char TAG[] = "BMP280";
const uint8_t Bmp280::s_i2c_addr = 0x76;

const char* Bmp280::s_measure_mode[4] = {"Sleep mode", "Forced mode", "Forced mode", "Normal mode"};
const char* Bmp280::s_oversampling[8] = {
        "Skipped", 
        "Oversamplig x 1", 
        "Oversamplig x 2", 
        "Oversamplig x 4", 
        "Oversamplig x 8",
        "Oversamplig x 16",
        "Oversamplig x 16",
        "Oversamplig x 16"
};

const char* Bmp280::s_standby_time[8] = {
    "0,5",
    "62,5",
    "125",
    "250",
    "500",
    "1000",
    "2000",
    "4000"
};


Bmp280::Bmp280(const std::string& name, 
    const std::shared_ptr<pirobot::i2c::I2C> i2c, 
    const uint8_t i2c_addr,
    const uint8_t mode,
    const uint8_t preasure_oversampling,
    const uint8_t temperature_oversampling,
    const uint8_t standby_time,
    const uint8_t filter,
    const int spi,
    const int spi_channel,
    const std::string& comment) : 
    Item(name, comment, ItemTypes::BMP280), _i2caddr(i2c_addr), m_mode(mode), 
    m_preasure_oversampling(preasure_oversampling), m_temperature_oversampling(temperature_oversampling), 
    m_standby_time(standby_time), m_filter(filter), m_spi(spi), m_spi_channel(spi_channel){

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));

    //register I2C user
    i2c->add_user(name, _i2caddr);

    I2CWrapper::lock();
    m_fd = I2CWrapper::I2CSetup(_i2caddr);
    I2CWrapper::unlock();

    /*
    * 1. Switch to SLEEP and set Measure control parameters
    * 2. Set configuration parameters
    * 3. If mode is NORMAL switch this mode On
    */
    set_measure_control(BMP280_POWER_MODE_SLEEP, m_preasure_oversampling, m_temperature_oversampling);
    set_config(m_spi, m_filter, m_standby_time);
    if(m_mode == BMP280_POWER_MODE_NORMAL)
        set_measure_control(m_mode, m_preasure_oversampling, m_temperature_oversampling);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));
}

//
Bmp280::~Bmp280(){

}

//Get chip ID 
uint8_t Bmp280::get_id(){
    I2CWrapper::lock();
    uint8_t id = I2CWrapper::I2CReadReg8(m_fd, BMP280_ID);
    I2CWrapper::lock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ID: " + std::to_string(id));
    return id;    
}

//Reset
void Bmp280::reset(){
    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, BMP280_RESET, BMP280_RESET_CODE);
    I2CWrapper::lock();
}

//Get status
const uint8_t Bmp280::get_status(){
    I2CWrapper::lock();
    uint8_t status = I2CWrapper::I2CReadReg8(m_fd, BMP280_STATUS);
    I2CWrapper::lock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Status Measuring: " + ((status & BMP280_STATUS_MEASURING) ? "Yes" : "No") + 
                    "  Data update: " + ((status & BMP280_STATUS_DATA_UPDATE) ? "Yes" : "No"));
    return status;    
}

//Set measure control
bool Bmp280::set_measure_control(const uint8_t power_mode, const uint8_t pressure_over, const uint8_t temp_over){
    assert(power_mode <= 3);
    assert(pressure_over <= 8);
    assert((temp_over <= 8 || temp_over == 0xFF));

    if((power_mode > 3) | (pressure_over > 8) | (temp_over > 8 && temp_over != 0xFF) | (pressure_over == BMP280_OVERSAMPLING_SKIPPED && temp_over == 0xFF))
        return false;

    uint8_t temp_over_val = temp_over;
    if(temp_over_val == 0xFF){
        temp_over_val = (pressure_over >= 5 ? BMP280_OVERSAMPLING_2 : BMP280_OVERSAMPLING_1);
    }

    uint8_t meas_ctrl = (temp_over_val << 5) | (pressure_over << 2) | power_mode;

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, BMP280_CTRL_MEASURE, meas_ctrl);
    I2CWrapper::lock();
    
    return true;
}

//Get measure control parameters
const uint8_t Bmp280::get_measure_control(){
    I2CWrapper::lock();
    uint8_t meas_ctrl = I2CWrapper::I2CReadReg8(m_fd, BMP280_CTRL_MEASURE);
    I2CWrapper::lock();
    
    uint8_t mode = (meas_ctrl & 0x03);
    uint8_t osrs_p = ((meas_ctrl >> 2) & 0x07);
    uint8_t osrs_t = (meas_ctrl >> 5);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Measure control. Power mode: " + s_measure_mode[mode] + 
            " Oversampling Preassure: " + s_oversampling[osrs_p] + " Temperature: " + s_oversampling[osrs_t]);
    return meas_ctrl;
}

//Get config
const uint8_t Bmp280::get_config(){
    I2CWrapper::lock();
    uint8_t config = I2CWrapper::I2CReadReg8(m_fd, BMP280_CONFIG);
    I2CWrapper::lock();

    uint8_t spi = (config & 0x01);
    uint8_t filter = ((config >> 2) & 0x07);
    uint8_t standby_time = (config >> 5);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Config. SPI: " + (spi ? "On" : "Off") + 
            " Filter: " + std::to_string(filter) + " Temperature: " + s_standby_time[standby_time] + " ms");
    
    return config;    
}

//Set config
void Bmp280::set_config(const uint8_t spi, const uint8_t filter, const uint8_t standby_time){

    int8_t config = (standby_time << 5) | (filter << 2) | spi;

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, BMP280_CONFIG, config);
    I2CWrapper::lock();
}

//Return current pressure and temperature values
void Bmp280::get_results(uint32_t& pressure, uint32_t& temp){
    uint8_t raw[6];
    I2CWrapper::lock();
    int read_bytes = I2CWrapper::I2CReadData(m_fd, BMP280_PRESSURE_MSB, raw, 6);
    I2CWrapper::lock();
    
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Read bytes: " + std::to_string(read_bytes));

    pressure = construct_value(raw[0], raw[1], raw[2]);
    temp = construct_value(raw[3], raw[4], raw[5]);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Pressure: " + std::to_string(pressure) + " Temperature: " + std::to_string(temp));
}


}//item
}//pirobot