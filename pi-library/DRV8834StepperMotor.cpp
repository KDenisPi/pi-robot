/*
 * DRV8834StepperMotor.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: Denis Kudia
 */
#include <thread>

#include "DRV8834StepperMotor.h"
#include "logger.h"

const char TAG[] = "ST_8834";


namespace pirobot {
namespace item {

/*
 *
 */
DRV8834_StepperMotor::DRV8834_StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
		const std::string name,
		const std::string comment) :
	Item(gpio, name, comment, ItemTypes::STEPPER),
	m_gpio_enable(nullptr),
	m_gpio_sleep(nullptr),
	m_gpio_mode_0(nullptr),
	m_gpio_mode_1(nullptr),
	m_gpio_dir(nullptr),
	m_enable(true),
	m_sleep(false),
	m_direction(MOTOR_DIR::DIR_CLOCKWISE),
	m_step_size(DRV8834_MICROSTEP::STEP34_FULL)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

}


DRV8834_StepperMotor::~DRV8834_StepperMotor() {
	// TODO Auto-generated destructor stub
}

/*
 * Set GPIO.
 * Note: GPIO should have OUT mode
 */
void DRV8834_StepperMotor::set_gpio(const std::shared_ptr<pirobot::gpio::Gpio> gpio, const DRV88_PIN pin){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set GPIO for pin: " + std::to_string(pin));

	assert(gpio.get() != nullptr);
	assert(gpio->getMode() ==  gpio::GPIO_MODE::OUT);

	switch(pin){
		case PIN_ENABLE:
			m_gpio_enable = gpio;
			return;
		case PIN_SLEEP:
			m_gpio_sleep = gpio;
			return;
		case PIN_MODE_0:
			m_gpio_mode_0 = gpio;
			return;
		case PIN_MODE_1:
			m_gpio_mode_1 = gpio;
			return;
		case PIN_DIR:
			m_gpio_dir = gpio;
			return;
	}
}

/*
 * Set direction
 */
void DRV8834_StepperMotor::set_direction(const MOTOR_DIR direction){
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
void DRV8834_StepperMotor::set_enable(const bool enable){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Enable: " + std::to_string(enable));
	assert(m_gpio_enable.get() != nullptr);

	if(m_gpio_enable.get() == nullptr)
		return;

	m_gpio_enable->digitalWrite((enable ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH));
	m_enable = enable;
}

/*
 * Logic high to enable device, logic low to enter low-power sleep mode.
 */
void DRV8834_StepperMotor::set_sleep(const bool sleep){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Sleep: " + std::to_string(sleep));
	assert(m_gpio_sleep.get() != nullptr);

	if(m_gpio_sleep.get() == nullptr)
		return;

	m_gpio_sleep->digitalWrite((sleep ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH));
	m_sleep = sleep;
}


/*
 * Set Step Size
 *
 *
 * 	M0			M1		Microstep Resolution
 * 	----------------------------------------
 *	Low			Low		Full step
 *	High		Low		Half step
 *	Floating	Low		1/4 step
 *	Low			High	1/8 step
 *	High		High	1/16 step
 *	Floating	High	1/32 step
 */
void DRV8834_StepperMotor::set_step_size(const DRV8834_MICROSTEP step_size){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Set Step Size: " + std::to_string(step_size));

	assert(m_gpio_mode_1.get() != nullptr);

	if((m_gpio_mode_0.get() == nullptr))
	{
		if(step_size != DRV8834_MICROSTEP::STEP34_1_4 && step_size != DRV8834_MICROSTEP::STEP34_1_32){
			//TODO: Throw exception Invalid parameter there
			return;
		}
		gpio::SGN_LEVEL mode1 = ((step_size == DRV8834_MICROSTEP::STEP34_1_4) ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);
		m_gpio_mode_1->digitalWrite(mode1);
	}
	else{
		if(step_size == DRV8834_MICROSTEP::STEP34_1_4 || step_size == DRV8834_MICROSTEP::STEP34_1_32){
			//TODO: Throw exception Invalid parameter there
			return;
		}
		assert(m_gpio_mode_0.get() != nullptr);

		gpio::SGN_LEVEL mode0 = ( (step_size & 0x01) == 0 ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);
		gpio::SGN_LEVEL mode1 = ( (step_size & 0x02) == 0 ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);

		m_gpio_mode_0->digitalWrite(mode0);
		m_gpio_mode_1->digitalWrite(mode1);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	//delay(50);
	m_step_size = step_size;
}

/*
 *
 */
bool DRV8834_StepperMotor::initialize(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started ");

	set_direction(m_direction);
	set_step_size(m_step_size);
	set_sleep(m_sleep);
	set_enable(m_enable);
}

/*
 * Move on step(s)
 * No more than 10 steps
 */
void DRV8834_StepperMotor::step(const int num_steps /*= 1*/){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Move on step: " + std::to_string(num_steps));

	int steps = (num_steps > 10 ? 10 : num_steps);
	auto fsteps = [this](int steps, std::string& name) -> void {
		logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Thread started for: " + name);
		for(int i = 0; i < steps && this->is_rotate(); i++){
			this->get_gpio()->digitalWrite(gpio::SGN_LEVEL::SGN_HIGH);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			//delay(50);
			this->get_gpio()->digitalWrite(gpio::SGN_LEVEL::SGN_LOW);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			//delay(50);
		}

		//if rotation have not finished by external reason - send message
		if(this->is_rotate()){
			unsigned int state = GENERAL_NTFY::GN_DONE;
			this->notify(this->type(), name, (void*)(&state));
		}

		logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Thread finished for: " + name);
	};

	std::string name = this->name();
	std::thread t(fsteps, steps, std::ref(name));
	t.detach();
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished ");
}

/*
 *
 */
const std::string DRV8834_StepperMotor::to_string(){
	return name();
}

/*
 *
 */
const std::string DRV8834_StepperMotor::printConfig(){
	std::string conf =  name() + "\n" +
			" GPIO Step: " + get_gpio()->to_string() + "\n";
	conf += " GPIO Sleep: " + (m_gpio_sleep.get() == nullptr ? "Not Defined" : m_gpio_sleep->to_string()) + "\n";
	conf += " GPIO Direction: " + (m_gpio_dir.get() == nullptr ? "Not Defined" : m_gpio_dir->to_string()) + "\n";
	conf += " GPIO Enable: " + (m_gpio_enable.get() == nullptr ? "Not Defined" : m_gpio_enable->to_string()) + "\n";
	conf += " GPIO Mode0: " + (m_gpio_mode_0.get() == nullptr ? "Not Defined" : m_gpio_mode_0->to_string()) + "\n";
	conf += " GPIO Mode1: " + (m_gpio_mode_1.get() == nullptr ? "Not Defined" : m_gpio_mode_1->to_string()) + "\n";
	conf = conf +
			" Enable: " + std::to_string(m_enable) +
			" Sleep: " + std::to_string(m_sleep) +
			" Direction: " + std::to_string(m_direction) +
			" Step size: " + std::to_string(m_step_size);
	return conf;
}


} /* namespace item */
} /* namespace pirobot */
