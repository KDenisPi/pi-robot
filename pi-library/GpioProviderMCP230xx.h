/*
 * GpioProviderMCP230xx.h
 *
 *  Created on: Nov 23, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERMCP230xx_H_
#define PI_LIBRARY_GPIOPROVIDERMCP230xx_H_

#include <memory>

#include <mcp23x0817.h>

#include "I2C.h"
#include "I2CWrapper.h"
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

    // Return current state of GPIOs
    uint8_t dgtRead8(const int pin = 0){
       return get_OLAT(pin);
    }

    //Write 8-bit value
    void dgtWrite8(const uint8_t value, const int pin = 0){
        int gpio = get_GPIO_addr(pin);

        I2CWrapper::lock();
        I2CWrapper::I2CWriteReg8(m_fd, gpio, value);
        I2CWrapper::unlock();

        set_OLAT(value, pin);
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
    virtual void set_OLAT(const unsigned int value, const int pin) = 0;

protected:
    uint8_t _i2caddr;
    int m_fd;
};

} /* namespace gpio */
} /* namespace pirobot */

#endif /* PI_LIBRARY_GPIOPROVIDERMCP230xx_H_ */
