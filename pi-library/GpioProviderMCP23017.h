/*
 * GpioProviderMCP23017.h
 *
 *  Created on: Nov 23, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERMCP23017_H_
#define PI_LIBRARY_GPIOPROVIDERMCP23017_H_

#include <memory>
#include "I2C.h"
#include "GpioProvider.h"

namespace pirobot {
namespace gpio {

class GpioProviderMCP23017: public GpioProvider {
public:
    GpioProviderMCP23017(const std::string name, std::shared_ptr<pirobot::i2c::I2C> i2c, const uint8_t i2c_addr = s_i2c_addr, const int pins = s_pins);
    virtual ~GpioProviderMCP23017();

    /*
    *
    */
    virtual const std::string to_string() override {
        return "Name: " + get_name() + " Type:" + std::string("MCP23017") + " I2C addr: " + std::to_string(_i2caddr);
    }

    virtual const int dgtRead(const int pin) override;
    virtual void dgtWrite(const int pin, const int value) override;
    virtual void setmode(const int pin, const gpio::GPIO_MODE mode) override;
    virtual void pullUpDownControl(const int pin, const gpio::PULL_MODE pumode) override;
    virtual const GPIO_PROVIDER_TYPE get_type() const override { return GPIO_PROVIDER_TYPE::PROV_MCP23017; }

    virtual const std::string printConfig() override {
        return to_string() + "\n";
    }

	static const int s_pins;
    static const uint8_t s_i2c_addr;

private:
      uint8_t _i2caddr;
      int m_fd;

      unsigned int m_OLATA;
      unsigned int m_OLATB;
};

} /* namespace gpio */
} /* namespace pirobot */

#endif /* PI_LIBRARY_GPIOPROVIDERMCP23017_H_ */
