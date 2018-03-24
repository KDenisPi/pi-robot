/*
 * sgp30.h
 * I2C Sensurion Gas Platform
 *
 *  Note: This device always uses 0x58 address
 * 
 *  Created on: Feb 21, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SGP30_H
#define PI_LIBRARY_SGP30_H

#include "item.h"
#include "I2C.h"
#include "Threaded.h"
#include "crc.h"

namespace pirobot {
namespace item {

#define SGP30_GET_FEATURE_SET_VERSION       0x202F
#define SGP30_INIT_AIR_QUALITY              0x2003
#define SGP30_MEASURE_AIR_QUALITY           0x2008
#define SGP30_GET_BASELINE                  0x2015
#define SGP30_SET_BASELINE                  0x201E
#define SGP30_SET_HUMIDITY                  0x2061
#define SGP30_MEASURE_TEST                  0x2032

#define SGP30_MEASURE_TEST_PATTERN          0xD400

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

    void start(){
        piutils::Threaded::start<pirobot::item::Sgp30>(this);
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
    // Set baseline
    void set_baseline(const uint16_t base_CO2, const uint16_t base_TVOC);
    //Set humidity
    void set_humidity(const uint16_t humidity);
    //Execute measure test, true - chip is OK
    bool measure_test();

private:
    uint8_t _i2caddr;
    int m_fd;

    std::mutex cv_m_data;
    std::condition_variable cv_data;

    //lest measured values
    struct Sdp30_measure values;
    struct Sdp30_measure baseline;
    uint16_t _humidity;

    // Get feature set version
    void get_feature_set_version();

    // Read data 
    int read_data(uint8_t* data, const int len, const uint16_t cmd, const int delay);
    int write_data(uint8_t* data, const int len, const uint16_t cmd, const int delay);

    //Convert two byte value to uint16_t
    uint16_t convert_result(uint8_t* data){
        uint16_t result = data[0];
        result = (result << 8 ) | data[1];
        return result;
    }

    // Set baseline
    void _set_baseline();
    //Set humidity
    void _set_humidity();

public:
    //Get measure results
    void get_results(uint16_t& co2, uint16_t& tvoc);
    //Get measure results
    void get_baseline(uint16_t& bs_co2, uint16_t& bs_tvoc);
};

}//item
}//pirobot

#endif
