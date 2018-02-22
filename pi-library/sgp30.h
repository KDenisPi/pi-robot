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

#ifndef PI_LIBRARY_SGP30_H
#define PI_LIBRARY_SGP30_H

namespace pirobot {
namespace item {

#define SGP30_GET_FEATURE_SET_VERSION_1   0x20
#define SGP30_GET_FEATURE_SET_VERSION_2   0x2F

class Sgp30 : public item::Item, public piutils::Threaded {

public:
    Sgp30(const std::string& name, const std::shared_ptr<pirobot::i2c::I2C> i2c, const std::string& comment = "");
    virtual ~Sgp30();

    static const uint8_t s_i2c_addr;

    virtual const std::string printConfig() override{
        return std::string(" SGP30 ");
    }

    /*
    *
    */
    virtual bool initialize() override{
        return piutils::Threaded::start<pirobot::item::Sgp30>(this);
    }

    /*
    * Worker function
    */
    static void worker(Sgp30* p);

    //
    virtual void stop() override;

private:
    uint8_t _i2caddr;
    int m_fd;

    void get_feature_set_version();
};

}//item
}//pirobot

#endif