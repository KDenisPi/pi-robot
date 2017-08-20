/*
 * GpioProviderPCA9685.h
 *
 *  Created on: Nov 14, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERPCA9685_H_
#define PI_LIBRARY_GPIOPROVIDERPCA9685_H_

#include <memory>
#include "GpioProvider.h"
#include "Adafruit_PWMServoDriver.h"

namespace pirobot {
namespace gpio {

class GpioProviderPCA9685: public GpioProvider {
public:
    GpioProviderPCA9685(const std::string name, std::shared_ptr<Adafruit_PWMServoDriver> pwm, 
        const int pin_start=DEFAULT_PIN_START::PROV_PIN_PCA9685, const float freq = 60.0);
    virtual ~GpioProviderPCA9685();

    virtual const std::string to_string() override;
    virtual const int dgtRead(const int pin) override;
    virtual void dgtWrite(const int pin, const int value) override;
    virtual void dgtWritePWM(const int pin, const float dutyCycle, const float phaseOffset) override;
    virtual void setmode(const int pin, const gpio::GPIO_MODE mode) override;
    virtual void pullUpDownControl(const int pin, const gpio::PULL_MODE pumode) override;
    virtual void setPulse(const int pin, const uint16_t pulselen) override;
    virtual const GPIO_PROVIDER_TYPE get_type() const override { return GPIO_PROVIDER_TYPE::PROV_PCA9685; }

private:
    std::shared_ptr<Adafruit_PWMServoDriver> m_pwm;
    float m_freq;
};

} /* namespace gpio */
} /* namespace pirobot */

#endif /* PI_LIBRARY_GPIOPROVIDERPCA9685_H_ */
