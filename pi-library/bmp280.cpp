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
    const uint8_t pressure_oversampling,
    const uint8_t temperature_oversampling,
    const uint8_t standby_time,
    const uint8_t filter,
    const int spi,
    const int spi_channel,
    const std::string& comment) :
    Item(name, comment, ItemTypes::BMP280), _i2caddr(i2c_addr), m_mode(mode),
    m_pressure_oversampling(pressure_oversampling), m_temperature_oversampling(temperature_oversampling),
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
    * 3. Read compensation values
    * 4. If mode is NORMAL switch this mode On
    */
    set_measure_control(BMP280_POWER_MODE_SLEEP, m_pressure_oversampling, m_temperature_oversampling);
    set_config(m_spi, m_filter, m_standby_time);
    read_compensation();
    if(m_mode == BMP280_POWER_MODE_NORMAL)
        set_measure_control(m_mode, m_pressure_oversampling, m_temperature_oversampling);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));
}

//
Bmp280::~Bmp280(){

}

//Get chip ID
uint8_t Bmp280::get_id(){
    I2CWrapper::lock();
    uint8_t id = I2CWrapper::I2CReadReg8(m_fd, BMP280_ID);
    I2CWrapper::unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ID: " + std::to_string(id));
    return id;
}

//Reset
void Bmp280::reset(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, BMP280_RESET, BMP280_RESET_CODE);
    I2CWrapper::unlock();
}

//Get status
const uint8_t Bmp280::get_status(){
    I2CWrapper::lock();
    uint8_t status = I2CWrapper::I2CReadReg8(m_fd, BMP280_STATUS);
    I2CWrapper::unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Status Measuring: " + ((status & BMP280_STATUS_MEASURING) ? "Yes" : "No") +
                    "  Data update: " + ((status & BMP280_STATUS_DATA_UPDATE) ? "Yes" : "No"));
    return status;
}

//Set measure control
bool Bmp280::set_measure_control(const uint8_t power_mode, const uint8_t pressure_over, const uint8_t temp_over){
    assert(power_mode <= 3);
    assert(pressure_over <= 8);
    assert((temp_over <= 8 || temp_over == 0xFF));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Mode: " + std::to_string(power_mode) + " osrs_p: " + std::to_string(pressure_over) + " osrs_t: " + std::to_string(temp_over));

    if((power_mode > 3) | (pressure_over > 8) | (temp_over > 8 && temp_over != 0xFF) | (pressure_over == BMP280_OVERSAMPLING_SKIPPED && temp_over == 0xFF))
        return false;

    uint8_t temp_over_val = temp_over;
    if(temp_over_val == 0xFF){
        temp_over_val = (pressure_over >= 5 ? BMP280_OVERSAMPLING_2 : BMP280_OVERSAMPLING_1);
    }

    uint8_t meas_ctrl = (temp_over_val << 5) | (pressure_over << 2) | power_mode;

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, BMP280_CTRL_MEASURE, meas_ctrl);
    I2CWrapper::unlock();

    return true;
}

//Get measure control parameters
const uint8_t Bmp280::get_measure_control(){
    I2CWrapper::lock();
    uint8_t meas_ctrl = I2CWrapper::I2CReadReg8(m_fd, BMP280_CTRL_MEASURE);
    I2CWrapper::unlock();

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
    I2CWrapper::unlock();

    uint8_t spi = (config & 0x01);
    uint8_t filter = ((config >> 2) & 0x07);
    uint8_t standby_time = (config >> 5);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Config. SPI: " + (spi ? "On" : "Off") +
            " Filter: " + std::to_string(filter) + " Temperature: " + s_standby_time[standby_time] + " ms");

    return config;
}

//Set config
void Bmp280::set_config(const uint8_t spi, const uint8_t filter, const uint8_t standby_time){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " SPI: " + std::to_string(spi) + " Filter: " + std::to_string(filter) + " SB Time: " + std::to_string(standby_time));

    int8_t config = (standby_time << 5) | (filter << 2) | spi;

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, BMP280_CONFIG, config);
    I2CWrapper::unlock();
}

//Return current pressure and temperature values
void Bmp280::get_results(float& pressure, float& temp, float& altitude){
    uint8_t raw[6];

    //for FORCED ans SLEEP modes make measurement
    if(m_mode != BMP280_POWER_MODE_NORMAL)
        set_measure_control(m_mode, m_pressure_oversampling, m_temperature_oversampling);

    I2CWrapper::lock();
    int read_bytes = I2CWrapper::I2CReadData(m_fd, BMP280_PRESSURE_MSB, raw, 6);
    I2CWrapper::unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Read bytes: " + std::to_string(read_bytes));

    int32_t raw_pressure = construct_value(raw[0], raw[1], (raw[2] >> 4));
    int32_t raw_temp = construct_value(raw[3], raw[4], (raw[5] >> 4));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " RAW Pressure: " + std::to_string(raw_pressure) + " Temperature: " + std::to_string(raw_temp));

    //order is important - we use temperature value for pressure calculation
    temp = std::round((calculate_Temperature(raw_temp)*10))/10;
    pressure = std::round(calculate_Pressure(raw_pressure)/133.3);
    altitude = std::round(read_Altitude(pressure));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Pressure: " + std::to_string(pressure) + " Temperature: " + std::to_string(temp) + 
        " Altitude: " + std::to_string(altitude));
}

//Read compensation value
void Bmp280::read_compensation(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));

    I2CWrapper::lock();

    dig_T1 = (uint16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_T1);
    dig_T2 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_T2);
    dig_T3 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_T3);

    dig_P1 = (uint16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_P1);
    dig_P2 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_P2);
    dig_P3 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_P3);
    dig_P4 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_P4);
    dig_P5 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_P5);
    dig_P6 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_P6);
    dig_P7 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_P7);
    dig_P8 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_P8);
    dig_P9 = (int16_t)I2CWrapper::I2CReadReg16(m_fd, BMP280_dig_P9);

    I2CWrapper::unlock();
}

//calculate temperature
float Bmp280::calculate_Temperature(int32_t temperature)
{
    int32_t var1, var2;

    var1  = ((((temperature>>3) - ((int32_t)dig_T1 <<1))) * ((int32_t)dig_T2)) >> 11;
    var2  = (((((temperature>>4) - ((int32_t)dig_T1)) * ((temperature>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    float T  = (t_fine * 5 + 128) >> 8;
    return T/100;
}

//calculate pressure
float Bmp280::calculate_Pressure(int32_t pressure){
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
    var2 = var2 + (((int64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) +
        ((var1 * (int64_t)dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;

    if (var1 == 0) {
        return 0;  // avoid exception caused by division by zero
    }

    p = 1048576 - pressure;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
    return (float)p/256;
}

//calculate altitude
float Bmp280::read_Altitude(const float pressure, const float seaLevelhPa){
  float altitude;
  altitude = 44330 * (1.0 - pow((pressure/100) / seaLevelhPa, 0.1903));
  return altitude;
}

}//item
}//pirobot
