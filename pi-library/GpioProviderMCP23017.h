/*
 * GpioProviderMCP23017.h
 *
 *  Created on: Nov 23, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERMCP23017_H_
#define PI_LIBRARY_GPIOPROVIDERMCP23017_H_

#include <memory>

#include <mcp23x0817.h>

#include "I2C.h"
#include "GpioProvider.h"

namespace pirobot {
namespace gpio {

class GpioProviderMCP230xx: public GpioProvider{
public:
    GpioProviderMCP230xx(const std::string& name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr, const int pins);
    virtual ~GpioProviderMCP230xx();

    virtual const int dgtRead(const int pin) override;
    virtual void dgtWrite(const int pin, const int value) override;
    virtual void setmode(const int pin, const gpio::GPIO_MODE mode) override;
    virtual void pullUpDownControl(const int pin, const gpio::PULL_MODE pumode) override;

    virtual const std::string printConfig() override {
        return to_string() + "\n";
    }

private:
    //Get address for GPIO register
    virtual const uint8_t get_GPIO_addr(const int pin) = 0;
    //Get address IODIR (IO direction) register
    virtual const uint8_t get_IODIR_addr(const int pin) = 0;
    //Get address GPPU (Pull-Up) register
    virtual const uint8_t get_GPPU_addr(const int pin) = 0;

    //Get current value for register data
    virtual unsigned int get_OLAT(const int pin) = 0;
    //Save current value register data
    virtual void set_OLAT(const int pin, const unsigned int value) = 0;

protected:
    uint8_t _i2caddr;
    int m_fd;
};

//
//Class for MCP23017 (I2C GPIO extender with 16 pins)
//
class GpioProviderMCP23017: public GpioProviderMCP230xx {
public:
    GpioProviderMCP23017(const std::string& name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr = s_i2c_addr, const int pins = s_pins);
    virtual ~GpioProviderMCP23017();

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
        return (pin < 8 ? MCP23x17_GPIOA : MCP23x17_GPIOB);
    }

    //Get address IODIR (IO direction) register
    virtual const uint8_t get_IODIR_addr(const int pin){
        return (pin < 8 ? MCP23x17_IODIRA : MCP23x17_IODIRB);
    }

    //Get address GPPU (Pull-Up) register
    virtual const uint8_t get_GPPU_addr(const int pin){
        return (pin < 8 ? MCP23x17_GPPUA : MCP23x17_GPPUB);
    }

    //Get current value for register data
    virtual unsigned int get_OLAT(const int pin){
        return (pin < 8 ? m_OLATA : m_OLATB);
    }
    //Save current value register data
    virtual void set_OLAT(const int pin, const unsigned int value){
        if(pin < 8) m_OLATA = value;
         else m_OLATB = value;
    }

    unsigned int m_OLATA;
    unsigned int m_OLATB;
};

} /* namespace gpio */
} /* namespace pirobot */

#endif /* PI_LIBRARY_GPIOPROVIDERMCP23017_H_ */
