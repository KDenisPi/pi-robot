/*
 * DCMotor.cpp
 *
 *  Created on: Jan 10, 2017
 *      Author: denis
 */

#include <cassert>
#include <unistd.h>

#include "DCMotor.h"
#include "logger.h"

const char TAG[] = "DCMotor";

namespace pirobot {
namespace item {
namespace dcmotor {

DCMotor::DCMotor(const std::shared_ptr<pirobot::item::Drv8835> drv8835,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_direction,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_pwm,
		const MOTOR_DIR direction) :
				Item(gpio_direction, ItemTypes::DCMotor),
				m_drv8835(drv8835),
				m_gpio_pwm(gpio_pwm),
				m_direction(direction)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(gpio_pwm.get() != NULL);
	assert(gpio_pwm->getMode() ==  gpio::GPIO_MODE::OUT);

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

	set_direction(m_direction);
}

DCMotor::DCMotor(const std::shared_ptr<pirobot::item::Drv8835> drv8835,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_direction,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_pwm,
		const std::string name,
		const std::string comment,
		const MOTOR_DIR direction) :
				Item(gpio_direction, name, comment, ItemTypes::DCMotor),
				m_drv8835(drv8835),
				m_gpio_pwm(gpio_pwm),
				m_direction(direction)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(gpio_pwm.get() != NULL);
	assert(gpio_pwm->getMode() ==  gpio::GPIO_MODE::OUT);

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
	set_direction(m_direction);
	stop();
}


//set direction
//IN/IN mode: Logic high sets AOUT1 high
//PH/EN mode: Sets direction of H-bridge A
//For PH/EN mode: 0 - forward, 1 - reverse
void DCMotor::set_direction(const MOTOR_DIR direction){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Direction: " +
			(direction==MOTOR_DIR::DIR_CLOCKWISE ? "CLOCKWISE" : "COUTERCLOCKWISE"));

	get_gpio()->digitalWrite((direction == MOTOR_DIR::DIR_CLOCKWISE) ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);
	m_direction = direction;
}

/*
 *
 */
void DCMotor::set_power_level(const float level){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Level: " + std::to_string(level));

	float rlevel = level;

	if(level > 100.0f)
		rlevel = 100.0f;
	else if(level <= 0.0f)
		rlevel = 0.0f;

	m_gpio_pwm->digitalWritePWM(rlevel);
}

/*
 *
 */
void DCMotor::stop(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
	set_power_level(0.0f);
}


/*
 *
 */
DCMotor::~DCMotor() {
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
	stop();
}

/*
 *
 */
const std::string DCMotor::to_string(){
	return name();
}

const std::string DCMotor::printConfig(){
	std::string conf =  name() + "\n" +
			" GPIO Direction: " + get_gpio()->to_string() + "\n";
	conf += " GPIO PWM: " + (m_gpio_pwm.get() == nullptr ? "Not Defined" : m_gpio_pwm->to_string()) + "\n";
	conf += " Drv8835: " + (m_drv8835.get() == nullptr ? "Not Defined" : m_drv8835->printConfig()) + "\n";
	conf = conf + " Direction: " + std::to_string(m_direction);
	return conf;
}


} /* namespace dcmotor */
} /* namespace item */
} /* namespace pirobot */
