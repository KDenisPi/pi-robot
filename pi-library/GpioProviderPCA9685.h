/*
 * GpioProviderPCA9685.h
 *
 *  Created on: Nov 14, 2016
 *      Author: denis
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
	GpioProviderPCA9685(std::shared_ptr<Adafruit_PWMServoDriver> pwm);
	virtual ~GpioProviderPCA9685();

	virtual const std::string toString();
	virtual const int dgtRead(const int pin);
	virtual void dgtWrite(const int pin, const int value);
	virtual void setmode(const int pin, const gpio::GPIO_MODE mode);
	virtual void pullUpDownControl(const int pin, const gpio::PULL_MODE pumode);

private:
	std::shared_ptr<Adafruit_PWMServoDriver> m_pwm;
};

} /* namespace gpio */
} /* namespace pirobot */

#endif /* PI_LIBRARY_GPIOPROVIDERPCA9685_H_ */
