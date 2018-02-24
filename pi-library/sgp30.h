/*
 * sgp30.h
 * I2C Sensurion Gas Platform
 *
 *  Note: This device always uses 0x58 address
 * 
 *  Created on: Feb 21, 2018
 *      Author: Denis Kudia
 */

#include "item.h"
#include "I2C.h"
#include "Threaded.h"
#include "crc.h"

#ifndef PI_LIBRARY_SGP30_H
#define PI_LIBRARY_SGP30_H

namespace pirobot {
namespace item {

#define SGP30_GET_FEATURE_SET_VERSION       0x202F
#define SGP30_INIT_AIR_QUALITY              0x2003
#define SGP30_MEASURE_AIR_QUALITY           0x2008
#define SGP30_GET_BASELINE                  0x2015
#define SGP30_SET_BASELINE                  0x201E
#define SGP30_SET_HUMIDITY                  0x2061

struct Sdp30_measure {
    uint16_t uiCO2;
    uint16_t uiTVOC;    
};

class Sgp30 : public item::Item, public piutils::Threaded {

public:
    Sgp30(const std::string& name, const std::shared_ptr<pirobot::i2c::I2C> i2c, const std::string& comment = "");
    virtual ~Sgp30();

    static const uint8_t s_i2c_addr;

    virtual const std::string printConfig() override{
        return std::string(" SGP30 ");
    }

    // Initialize
    virtual bool initialize() override{
        return true; //piutils::Threaded::start<pirobot::item::Sgp30>(this);
    }

    // Worker function
    static void worker(Sgp30* p);

    // Stop
    virtual void stop() override;
    
    // Init air quality
    void init_air_quality();
    // Measure air quality
    void measure_air_quality();
    // Get baseline
    void get_baseline();
    //Set humidity
    void set_humidity(const uint16_t humidity);

private:
    uint8_t _i2caddr;
    int m_fd;

    //lest measured values
    struct Sdp30_measure values;
    struct Sdp30_measure baseline;

    // Get feature set version
    void get_feature_set_version();

    // Read data 
    void read_data(uint8_t* data, const int len, const uint16_t cmd, const int delay);

    // Save measure results
    void put_results(const uint16_t co2, const uint16_t tvoc){
        std::lock_guard<std::mutex> lk(cv_m);
    struct Sdp30_measure values;
        values.uiCO2 = co2;
        values.uiTVOC = tvoc;
    }

public:
    //Get measure results
    void get_results(uint16_t* results){
        std::lock_guard<std::mutex> lk(cv_m);
        results[0] = values.uiCO2;
        results[1] = values.uiTVOC;
    }
};

}//item
}//pirobot

#endif
