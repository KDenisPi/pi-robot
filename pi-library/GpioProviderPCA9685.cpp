/*
 * GpioProviderPCA9685.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: denis
 */

#include <cassert>

#include "logger.h"
#include "GpioProviderPCA9685.h"

namespace pirobot {
namespace gpio {

GpioProviderPCA9685::GpioProviderPCA9685(std::shared_ptr<Adafruit_PWMServoDriver> pwm) :
		GpioProvider(30), m_pwm(pwm)
{
	assert(pwm != nullptr);

}

GpioProviderPCA9685::~GpioProviderPCA9685() {
	// TODO Auto-generated destructor stub
}

const std::string GpioProviderPCA9685::toString(){
	return std::string("PCA9685");
}

/*
 * I am not sure that we have
 */
const int GpioProviderPCA9685::dgtRead(const int pin){
	struct LED_DATA data;

	m_pwm->getPin(getRealPin(pin), data);
	return data.off;
}

/*
 *
 */
void GpioProviderPCA9685::dgtWrite(const int pin, const int value){
	struct LED_DATA data;

	data.off = (value==1 ? 4095 : value);
	m_pwm->setPWM(getRealPin(pin), data.on, data.off);
}

void GpioProviderPCA9685::setmode(const int pin, const gpio::GPIO_MODE mode){
	/*
	 * TODO: It seems INPUT/OUTPUT is not implemented there
	 */
}
void GpioProviderPCA9685::pullUpDownControl(const int pin, const gpio::PULL_MODE pumode){
	/*
	 * TODO: It seems It is not implemented there
	 */
}


} /* namespace gpio */
} /* namespace pirobot */
