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

const char TAG[] = "PCA9685";

GpioProviderPCA9685::GpioProviderPCA9685(std::shared_ptr<Adafruit_PWMServoDriver> pwm, const float freq) :
		GpioProvider(30, 16), m_pwm(pwm), m_freq(freq)
{
	assert(pwm != nullptr);
	m_pwm->begin();
	m_pwm->setPWMFreq(m_freq);
}

GpioProviderPCA9685::~GpioProviderPCA9685() {
	// TODO Auto-generated destructor stub
}

const std::string GpioProviderPCA9685::to_string(){
	return std::string("PCA9685")+ " From: " + std::to_string(getStartPin());
}

/*
 * I am not sure that we have
 */
const int GpioProviderPCA9685::dgtRead(const int pin){
	struct LED_DATA data;
	m_pwm->getPin(getRealPin(pin), data);

        logger::log(logger::LLOG::DEBUD, TAG,
            std::string(__func__) + " Pin:" + std::to_string(pin) + " Value:" + std::to_string(data.off));

	return data.off;
}

/*
 *
 */
void GpioProviderPCA9685::dgtWrite(const int pin, const int value){
	struct LED_DATA data = {0, 0};

        logger::log(logger::LLOG::DEBUD, TAG, 
            std::string(__func__) + " Pin:" + std::to_string(pin) + "[" + std::to_string(getRealPin(pin)) +
		 "] Value:" + std::to_string(value));

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

/*
 *
 */
void GpioProviderPCA9685::setPulse(const int pin, const uint16_t pulselen){
	struct LED_DATA data = {0, 0};
	double pulse = 1000000 / (m_freq*4096);
	data.off =  static_cast<uint16_t>(pulselen/pulse);

	logger::log(logger::LLOG::DEBUD, TAG, std::string(__func__) + std::string(" to pin: ") + std::to_string(pin) +
			" pulselen(ms): " + std::to_string(pulselen) + " pulse: " + std::to_string(data.off));

	m_pwm->setPWM(getRealPin(pin), data.on, data.off);
}


} /* namespace gpio */
} /* namespace pirobot */
