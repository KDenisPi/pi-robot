/*
 * GpioProviderMCP23008.h
 *
 *  Created on: Nov 23, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERMCP23008_H_
#define PI_LIBRARY_GPIOPROVIDERMCP23008_H_

#include "logger.h"
#include "GpioProviderMCP230xx.h"

namespace pirobot {
namespace gpio {

enum MCP2308 : uint8_t {
    IODIR       = 0x00,
    IPOL        = 0x01,
    GPINTEN     = 0x02,
    DEFVAL      = 0x03,
    INTCON      = 0x04,
    IOCON       = 0x05,
    GPPU        = 0x06,
    INTF        = 0x07,
    INTCAP      = 0x08, // (Read-only)
    GPIO        = 0x09,
    OLAT        = 0x0A
};


//
//Class for MCP23008 (I2C GPIO extender with 8 pins)
//
class GpioProviderMCP23008: public GpioProviderMCP230xx {
public:
    GpioProviderMCP23008(const std::string& name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr = s_i2c_addr, const int pins = s_pins):
        GpioProviderMCP230xx(name, i2c, i2c_addr, pins), m_OLAT(0)
    {
        //
        // Confifure device and set initial values
        //
        _i2c->I2CWriteReg8(m_fd, MCP2308::IOCON, IOCON::INTPOL | IOCON::HAEN | IOCON::DISSLW | IOCON::SEQOP); //0x3A
        _i2c->I2CWriteReg8(m_fd, MCP2308::IODIR, 0x00);

        m_OLAT = _i2c->I2CReadReg8 (m_fd, MCP2308::GPIO);

        logger::log(logger::LLOG::DEBUG, "MCP23008", std::string(__func__) + " Descr: " + std::to_string(m_fd) + " ---> OLAT: " + std::to_string(m_OLAT));
    }

    //
    //
    //
    virtual ~GpioProviderMCP23008() {
        logger::log(logger::LLOG::DEBUG, "MCP23008", std::string(__func__));
    }

    /*
    *
    */
    virtual const std::string to_string() override {
        return "Name: " + get_name() + " Type:" + std::string("MCP23008") + " I2C addr: " + std::to_string(_i2caddr);
    }

    virtual const GPIO_PROVIDER_TYPE get_type() const override { return GPIO_PROVIDER_TYPE::PROV_MCP23008; }

    static const int s_pins;
    static const uint8_t s_i2c_addr;

private:
    //Get address for GPIO register
    virtual const uint8_t get_GPIO_addr(const int pin = -1){
        return MCP2308::GPIO;
    }

    //Get address IODIR (IO direction) register
    virtual const uint8_t get_IODIR_addr(const int pin = -1){
        return MCP2308::IODIR;
    }

    //Get address GPPU (Pull-Up) register
    virtual const uint8_t get_GPPU_addr(const int pin = -1){
        return MCP2308::GPPU;
    }

    //Get current value for register data
    virtual unsigned int get_OLAT(const int pin){
        m_OLAT = _i2c->I2CReadReg8 (m_fd, MCP2308::GPIO);
        return m_OLAT;
    }
    //Save current value register data
    virtual void set_OLAT(const unsigned int value, const int pin = -1){
        m_OLAT = value;
        //logger::log(logger::LLOG::DEBUG, "MCP23008", std::string(__func__) + " OLAT " + std::to_string(m_OLAT));
    }

    unsigned int m_OLAT;
};

const int GpioProviderMCP23008::s_pins = 8;
const uint8_t GpioProviderMCP23008::s_i2c_addr = 0x03;

} /* namespace gpio */
} /* namespace pirobot */

#endif /* PI_LIBRARY_GPIOPROVIDERMCP23008_H_ */
