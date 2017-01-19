/*
 * DRV8825StepperMotor.cpp
 *
 *  Created on: Jan 1, 2017
 *      Author: denis
 */

#include "DRV8825StepperMotor.h"
#include "logger.h"

#include <wiringPi.h>

const char TAG[] = "ST_8825";

namespace pirobot {
namespace item {

/*
 *
 */
DRV8825_StepperMotor::DRV8825_StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio) :
	Item(gpio, ItemTypes::STEPPER),
	m_gpio_enable(gpio),
	m_gpio_reset(nullptr),
	m_gpio_sleep(nullptr),
	m_gpio_dacay(nullptr),
	m_gpio_mode_0(nullptr),
	m_gpio_mode_1(nullptr),
	m_gpio_mode_2(nullptr),
	m_gpio_dir(nullptr),
	m_enable(true),
	m_reset(false),
	m_sleep(false),
	m_direction(MOTOR_DIR::DIR_CLOCKWISE),
	m_step_size(DRV8825_MICROSTEP::STEP_FULL),
	m_decay(DRV8825_DECAY::DECAY_MIXED)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

}

DRV8825_StepperMotor::DRV8825_StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
		const std::string name,
		const std::string comment) :
	Item(gpio, name, comment, ItemTypes::STEPPER),
	m_gpio_enable(nullptr),
	m_gpio_reset(nullptr),
	m_gpio_sleep(nullptr),
	m_gpio_dacay(nullptr),
	m_gpio_mode_0(nullptr),
	m_gpio_mode_1(nullptr),
	m_gpio_mode_2(nullptr),
	m_gpio_dir(nullptr),
	m_enable(true),
	m_reset(false),
	m_sleep(false),
	m_direction(MOTOR_DIR::DIR_CLOCKWISE),
	m_step_size(DRV8825_MICROSTEP::STEP_FULL),
	m_decay(DRV8825_DECAY::DECAY_MIXED)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

}

DRV8825_StepperMotor::~DRV8825_StepperMotor() {
	// TODO Auto-generated destructor stub
}

/*
 * Set GPIO.
 * Note: GPIO should have OUT mode
 */
void DRV8825_StepperMotor::set_gpio(const std::shared_ptr<pirobot::gpio::Gpio> gpio, const DRV8825_PIN pin){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set GPIO for pin: " + std::to_string(pin));

	assert(gpio.get() != nullptr);
	assert(gpio->getMode() ==  gpio::GPIO_MODE::OUT);

	switch(pin){
		case PIN_ENABLE:
			m_gpio_enable = gpio;
			return;
		case PIN_RESET:
			m_gpio_reset = gpio;
			return;
		case PIN_SLEEP:
			m_gpio_sleep = gpio;
			return;
		case PIN_DECAY:
			m_gpio_dacay = gpio;
			return;
		case PIN_MODE_0:
			m_gpio_mode_0 = gpio;
			return;
		case PIN_MODE_1:
			m_gpio_mode_1 = gpio;
			return;
		case PIN_MODE_2:
			m_gpio_mode_2 = gpio;
			return;
		case PIN_DIR:
			m_gpio_dir = gpio;
			return;
	}
}

/*
 * Set direction
 */
void DRV8825_StepperMotor::set_direction(const MOTOR_DIR direction){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Direction: " + std::to_string(direction));
	assert(m_gpio_dir.get() != nullptr);

	if(m_gpio_dir.get() == nullptr)
		return;

	if(direction == MOTOR_DIR::DIR_CLOCKWISE){
		m_gpio_dir->digitalWrite(gpio::SGN_LEVEL::SGN_HIGH);
	}
	else{
		m_gpio_dir->digitalWrite(gpio::SGN_LEVEL::SGN_LOW);
	}
	m_direction = direction;
}

/*
 * Logic high to disable device outputs and indexer operation, logic low to enable.
 */
void DRV8825_StepperMotor::set_enable(const bool enable){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Enable: " + std::to_string(enable));
	assert(m_gpio_enable.get() != nullptr);

	if(m_gpio_enable.get() == nullptr)
		return;

	m_gpio_enable->digitalWrite((enable ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH));
	m_enable = enable;
}

/*
 * Active-low reset input initializes the indexer logic and disables the H-bridge outputs.
 */
void DRV8825_StepperMotor::set_reset(const bool reset){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Reset: " + std::to_string(reset));
	assert(m_gpio_reset.get() != nullptr);

	if(m_gpio_reset.get() == nullptr)
		return;

	m_gpio_reset->digitalWrite((reset ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH));
	m_reset = reset;
}

/*
 * Logic high to enable device, logic low to enter low-power sleep mode.
 */
void DRV8825_StepperMotor::set_sleep(const bool sleep){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Sleep: " + std::to_string(sleep));
	assert(m_gpio_sleep.get() != nullptr);

	if(m_gpio_sleep.get() == nullptr)
		return;

	m_gpio_sleep->digitalWrite((sleep ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH));
	m_sleep = sleep;
}


/*
 * Set Step Size
 */
void DRV8825_StepperMotor::set_step_size(const DRV8825_MICROSTEP step_size){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Step Size: " + std::to_string(step_size));

	assert(m_gpio_mode_0.get() != nullptr);
	assert(m_gpio_mode_1.get() != nullptr);
	assert(m_gpio_mode_2.get() != nullptr);

	if( (m_gpio_mode_0.get() == nullptr) ||
			(m_gpio_mode_1.get() == nullptr) ||
			(m_gpio_mode_2.get() == nullptr)){
		return;
	}

	gpio::SGN_LEVEL mode0 = ( (step_size & 0x01) == 0 ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);
	gpio::SGN_LEVEL mode1 = ( (step_size & 0x02) == 0 ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);
	gpio::SGN_LEVEL mode2 = ( (step_size & 0x04) == 0 ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);

	m_gpio_mode_0->digitalWrite(mode0);
	m_gpio_mode_1->digitalWrite(mode1);
	m_gpio_mode_2->digitalWrite(mode2);

	delay(50);

	m_step_size = step_size;
}

/*
 *
 */
void DRV8825_StepperMotor::set_decay(const DRV8825_DECAY decay){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Decay: " + std::to_string(decay));

	assert( (decay == DRV8825_DECAY::DECAY_MIXED) || (m_gpio_dacay.get() != nullptr) );

	if((decay == DRV8825_DECAY::DECAY_MIXED) || (m_gpio_dacay.get() == nullptr)){
		m_decay = DRV8825_DECAY::DECAY_MIXED; //Opened
	}
	else{
		m_gpio_dacay->digitalWrite((DRV8825_DECAY::DECAY_SLOW == decay ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH));
		m_decay = decay;
	}
}

bool DRV8825_StepperMotor::initialize(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started ");

	set_direction(m_direction);
	set_step_size(m_step_size);

	set_enable(m_enable);
	set_reset(m_reset);
	set_sleep(m_sleep);
	set_decay(m_decay);
}

/*
 * Move on step(s)
 * No more than 10 steps
 */
void DRV8825_StepperMotor::step(const int num_steps /*= 1*/){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Move on step: " + std::to_string(num_steps));

	int steps = (num_steps > 10 ? 10 : num_steps);
	for(int i = 0; i < num_steps; i++){
		get_gpio()->digitalWrite(gpio::SGN_LEVEL::SGN_HIGH);
		delay(50);
		get_gpio()->digitalWrite(gpio::SGN_LEVEL::SGN_LOW);
		delay(50);
	}
}

/*
 *
 */
const std::string DRV8825_StepperMotor::to_string(){
	return name();
}

/*
 *
 */
const std::string DRV8825_StepperMotor::printConfig(){
	std::string conf =  name() + "\n" +
			" GPIO Step: " + get_gpio()->to_string() + "\n";
	conf += " GPIO Reset: " + (m_gpio_reset.get() == nullptr ? "Not Defined" : m_gpio_reset->to_string()) + "\n";
	conf += " GPIO Sleep: " + (m_gpio_sleep.get() == nullptr ? "Not Defined" : m_gpio_sleep->to_string()) + "\n";
	conf += " GPIO Direction: " + (m_gpio_dir.get() == nullptr ? "Not Defined" : m_gpio_dir->to_string()) + "\n";
	conf += " GPIO Enable: " + (m_gpio_enable.get() == nullptr ? "Not Defined" : m_gpio_enable->to_string()) + "\n";
	conf += " GPIO Decay: " + (m_gpio_dacay.get() == nullptr ? "Not Defined" : m_gpio_dacay->to_string()) + "\n";
	conf += " GPIO Mode0: " + (m_gpio_mode_0.get() == nullptr ? "Not Defined" : m_gpio_mode_0->to_string()) + "\n";
	conf += " GPIO Mode1: " + (m_gpio_mode_1.get() == nullptr ? "Not Defined" : m_gpio_mode_1->to_string()) + "\n";
	conf += " GPIO Mode2: " + (m_gpio_mode_2.get() == nullptr ? "Not Defined" : m_gpio_mode_2->to_string()) + "\n";
	conf = conf +
			" Reset: " + std::to_string(m_reset) +
			" Enable: " + std::to_string(m_enable) +
			" Sleep: " + std::to_string(m_sleep) +
			" Direction: " + std::to_string(m_direction) +
			" Step size: " + std::to_string(m_step_size) +
			" Decay: " + std::to_string(m_decay);
	return conf;
}


} /* namespace item */
} /* namespace pirobot */
