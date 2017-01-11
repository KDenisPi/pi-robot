/*
 * DCMotor.cpp
 *
 *  Created on: Jan 10, 2017
 *      Author: denis
 */

#include "DCMotor.h"
#include "logger.h"

namespace pirobot {
namespace item {

DCMotor::DCMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_mode,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_direction,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_pwm,
		DCMotor_MODE mode) :
				Item(gpio_mode, ItemTypes::DCMotor),
				m_gpio_dir(gpio_direction),
				m_gpio_pwm(gpio_pwm),
				m_mode(mode),
				m_direction(DCMotor_DIR::DIR_CLOCKWISE)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(m_gpio_dir.get() != NULL);
	assert(m_gpio_dir->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(gpio_pwm.get() != NULL);
	assert(gpio_pwm->getMode() ==  gpio::GPIO_MODE::OUT);

	//
	//DRV8835 page 5
	//

	//set mode
	//Logic low selects IN/IN mode
	//Logic high selects PH/EN mode
	get_gpio()->digitalWrite((mode == DCMotor_MODE::IN_IN) ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);

	set_direction(m_direction);
}

DCMotor::DCMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_mode,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_direction,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_pwm,
		const std::string name,
		const std::string comment,
		DCMotor_MODE mode) :
				Item(gpio_mode, name, comment, ItemTypes::DCMotor),
				m_gpio_dir(gpio_direction),
				m_gpio_pwm(gpio_pwm),
				m_mode(mode),
				m_direction(DCMotor_DIR::DIR_CLOCKWISE)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(m_gpio_dir.get() != NULL);
	assert(m_gpio_dir->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(gpio_pwm.get() != NULL);
	assert(gpio_pwm->getMode() ==  gpio::GPIO_MODE::OUT);

	//
	//DRV8835 page 5
	//

	//set mode
	//Logic low selects IN/IN mode
	//Logic high selects PH/EN mode
	get_gpio()->digitalWrite((mode == DCMotor_MODE::IN_IN) ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);

	set_direction(m_direction);
}


//set direction
//IN/IN mode: Logic high sets AOUT1 high
//PH/EN mode: Sets direction of H-bridge A
//For PH/EN mode: 0 - forward, 1 - reverse
void DCMotor::set_direction(const DCMotor_DIR direction){
	m_gpio_dir->digitalWrite((direction == DCMotor_DIR::DIR_CLOCKWISE) ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);
	m_direction = direction;
}

/*
 *
 */
void DCMotor::set_power_level(const float level){

	float rlevel = level;

	if(level > 100.0f)
		rlevel = 100.0f;
	else if(level < 0.0f)
		rlevel = 0.0f;

	m_gpio_pwm->digitalWritePWM(rlevel);
}

/*
 *
 */
void DCMotor::stop(){
	set_power_level(0.0f);
}


/*
 *
 */
DCMotor::~DCMotor() {
	// TODO Auto-generated destructor stub
}

} /* namespace item */
} /* namespace pirobot */
