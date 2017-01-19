/*
 * ULN2003StepperMotor.cpp
 *
 *  Created on: Jan 18, 2017
 *      Author: denis
 */

const char TAG[] = "ST_U2003";

#include "ULN2003StepperMotor.h"
#include "logger.h"

#include <wiringPi.h>

namespace pirobot {
namespace item {

ULN2003StepperMotor::ULN2003StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_0,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_1,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_2,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_3) :
			Item(gpio_0, ItemTypes::STEPPER),
			m_gpio_1(gpio_1),
			m_gpio_2(gpio_2),
			m_gpio_3(gpio_3),
			m_step(10),
			m_direction(MOTOR_DIR::DIR_CLOCKWISE)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

	assert(m_gpio_1.get() != NULL);
	assert(m_gpio_1->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(m_gpio_2.get() != NULL);
	assert(m_gpio_2->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(m_gpio_3.get() != NULL);
	assert(m_gpio_3->getMode() ==  gpio::GPIO_MODE::OUT);

}

ULN2003StepperMotor::ULN2003StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_0,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_1,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_2,
		const std::shared_ptr<pirobot::gpio::Gpio> gpio_3,
		const std::string name,
		const std::string comment)	:
			Item(gpio_0, name, comment, ItemTypes::STEPPER),
			m_gpio_1(gpio_1),
			m_gpio_2(gpio_2),
			m_gpio_3(gpio_3),
			m_step(10),
			m_direction(MOTOR_DIR::DIR_CLOCKWISE)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

	assert(m_gpio_1.get() != NULL);
	assert(m_gpio_1->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(m_gpio_2.get() != NULL);
	assert(m_gpio_2->getMode() ==  gpio::GPIO_MODE::OUT);
	assert(m_gpio_3.get() != NULL);
	assert(m_gpio_3->getMode() ==  gpio::GPIO_MODE::OUT);

}

/*
 *
 */
ULN2003StepperMotor::~ULN2003StepperMotor() {
	// TODO Auto-generated destructor stub
}

/*
 *
 */
void ULN2003StepperMotor::set_direction(const MOTOR_DIR direction){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Direction: " + std::to_string(direction));
	m_direction = direction;
}

/*
 *
 */
void ULN2003StepperMotor::step(const int num_steps){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Step: " + std::to_string(num_steps));

	if(num_steps <= 0){
		return;
	}

	auto estep = [this](const uint8_t step_cmd){
		this->get_gpio()->digitalWrite( ((step_cmd & 0x01) == 0 ? pirobot::gpio::SGN_LEVEL::SGN_LOW : pirobot::gpio::SGN_LEVEL::SGN_HIGH) );
		this->m_gpio_1->digitalWrite( ((step_cmd & 0x02) == 0 ? pirobot::gpio::SGN_LEVEL::SGN_LOW : pirobot::gpio::SGN_LEVEL::SGN_HIGH) );
		this->m_gpio_2->digitalWrite( ((step_cmd & 0x04) == 0 ? pirobot::gpio::SGN_LEVEL::SGN_LOW : pirobot::gpio::SGN_LEVEL::SGN_HIGH) );
		this->m_gpio_3->digitalWrite( ((step_cmd & 0x08) == 0 ? pirobot::gpio::SGN_LEVEL::SGN_LOW : pirobot::gpio::SGN_LEVEL::SGN_HIGH) );
		delay(10);
	};

	// I am usually working with 28BYJ-48 Stepper - It has 64 step per cycle
	// Well one cycle looks as reasonable value
	int steps = (num_steps <= 64 ? num_steps : 64);
	if(m_step == 10){
		m_step = (m_direction == MOTOR_DIR::DIR_CLOCKWISE ? 0 : 7);
	}
	for(int i = 0; i < num_steps; i++){
		estep(m_cmd[m_step]);
		m_step = get_next_step(m_step);
	}
}

/*
 *
 */
bool ULN2003StepperMotor::initialize(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started ");

}

/*
 *
 */
const std::string ULN2003StepperMotor::to_string(){
	return name();
}

/*
 *
 */
const std::string ULN2003StepperMotor::printConfig(){
	std::string conf =  name() + "\n" +
			" GPIO Blue (1): " + get_gpio()->to_string() + "\n";
	conf += " GPIO Pink (2): " + (m_gpio_1.get() == nullptr ? "Not Defined" : m_gpio_1->to_string()) + "\n";
	conf += " GPIO Yell (3): " + (m_gpio_2.get() == nullptr ? "Not Defined" : m_gpio_2->to_string()) + "\n";
	conf += " GPIO Orng (4): " + (m_gpio_3.get() == nullptr ? "Not Defined" : m_gpio_3->to_string()) + "\n";
	conf = conf +" Direction: " + std::to_string(m_direction);
	return conf;
}



} /* namespace item */
} /* namespace pirobot */
