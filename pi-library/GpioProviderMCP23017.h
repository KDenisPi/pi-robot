/*
 * GpioProviderMCP23017.h
 *
 *  Created on: Nov 23, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERMCP23017_H_
#define PI_LIBRARY_GPIOPROVIDERMCP23017_H_

#include "logger.h"
#include "GpioProviderMCP230xx.h"

namespace pirobot {
namespace gpio {

//
//Class for MCP23017 (I2C GPIO extender with 16 pins)
//
class GpioProviderMCP23017: public GpioProviderMCP230xx {
public:
    GpioProviderMCP23017(const std::string& name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr = s_i2c_addr, const int pins = s_pins):
        GpioProviderMCP230xx(name, i2c, i2c_addr, pins), m_OLATA(0), m_OLATB(0)
    {
        //
        // Confifure device and set initial values
        //
        _i2c->I2CWriteReg8(m_fd, MCP23X17_B0::IOCONA, IOCON::INTPOL | IOCON::HAEN | IOCON::DISSLW | IOCON::SEQOP); //0x3A
        _i2c->I2CWriteReg8(m_fd, MCP23X17_B0::IOCONB, IOCON::INTPOL | IOCON::HAEN | IOCON::DISSLW | IOCON::SEQOP); //0x3A);

        _i2c->I2CWriteReg8(m_fd, MCP23X17_B0::IODIRA, 0x00);
        _i2c->I2CWriteReg8(m_fd, MCP23X17_B0::IODIRB, 0x00);

        m_OLATA = _i2c->I2CReadReg8 (m_fd, MCP23X17_B0::GPIOA);
        m_OLATB = _i2c->I2CReadReg8 (m_fd, MCP23X17_B0::GPIOB);

        logger::log(logger::LLOG::DEBUG, "MCP23017", std::string(__func__) + " Descr: " + std::to_string(m_fd)+ " ---> OLATA: " + std::to_string(m_OLATA)+ " ---> OLATB: " + std::to_string(m_OLATB));
    }

    //
    //
    //
    virtual ~GpioProviderMCP23017() {
        logger::log(logger::LLOG::DEBUG, "MCP23008", std::string(__func__));
    }


    /*
    *
    */
    virtual const std::string to_string() override {
        return "Name: " + get_name() + " Type:" + std::string("MCP23017") + " I2C addr: " + std::to_string(_i2caddr);
    }

    virtual const GPIO_PROVIDER_TYPE get_type() const override { return GPIO_PROVIDER_TYPE::PROV_MCP23017; }

	static const int s_pins;
    static const uint8_t s_i2c_addr;

private:
    //Get address for GPIO register
    virtual const uint8_t get_GPIO_addr(const int pin){
        return (pin < 8 ? MCP23X17_B0::GPIOA : MCP23X17_B0::GPIOB);
    }

    //Get address IODIR (IO direction) register
    virtual const uint8_t get_IODIR_addr(const int pin){
        return (pin < 8 ? MCP23X17_B0::IODIRA : MCP23X17_B0::IODIRB);
    }

    //Get address GPPU (Pull-Up) register
    virtual const uint8_t get_GPPU_addr(const int pin){
        return (pin < 8 ? MCP23X17_B0::GPPUA : MCP23X17_B0::GPPUB);
    }

    //Get current value for register data
    virtual unsigned int get_OLAT(const int pin){
        return (pin < 8 ? m_OLATA : m_OLATB);
    }
    //Save current value register data
    virtual void set_OLAT(const unsigned int value, const int pin){
        if(pin < 8) m_OLATA = value;
         else m_OLATB = value;
    }

    unsigned int m_OLATA;
    unsigned int m_OLATB;
};

const int GpioProviderMCP23017::s_pins = 16;
const uint8_t GpioProviderMCP23017::s_i2c_addr = 0x20;

} /* namespace gpio */
} /* namespace pirobot */

#endif /* PI_LIBRARY_GPIOPROVIDERMCP23017_H_ */
