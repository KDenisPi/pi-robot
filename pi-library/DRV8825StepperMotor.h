/*
 * DRV8825StepperMotor.h
 *
 *  Created on: Jan 1, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_DRV8825STEPPERMOTOR_H_
#define PI_LIBRARY_DRV8825STEPPERMOTOR_H_

#include "item.h"
#include "motor.h"
#include "Drv8825_8834.h"

namespace pirobot {
namespace item {


/*
 *
 */
enum DRV8825_DECAY {
	DECAY_SLOW = 0,
	DECAY_HIGH = 1,
	DECAY_MIXED = 2
};

class DRV8825_StepperMotor: public Item, public Motor {
public:
	// GPIO on constructor is used for STEP command (pin 22)
	DRV8825_StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio);

	DRV8825_StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment);

	virtual ~DRV8825_StepperMotor();

	void set_gpio(const std::shared_ptr<pirobot::gpio::Gpio> gpio, const DRV88_PIN pin);
	void set_direction(const MOTOR_DIR dir);
	void set_enable(const bool enable);
	void set_reset(const bool reset);
	void set_sleep(const bool sleep);
	void set_decay(const DRV8825_DECAY decay);

	void set_step_size(const DRV8825_MICROSTEP step_size);

	void step(const int num_steps = 1);

	inline const bool is_enable() const {return m_enable;}
	inline const bool is_reset() const {return m_reset;}
	inline const bool is_sleep() const {return m_sleep;}

	inline const MOTOR_DIR direction() const {return m_direction;}
	inline const DRV8825_MICROSTEP step_size() const {return m_step_size;}
	inline const DRV8825_DECAY decay() const {return m_decay;}

	virtual bool initialize() override;
	virtual const std::string to_string() override;
	virtual const std::string printConfig() override;


private:

	bool m_enable;
	bool m_reset;
	bool m_sleep;
	MOTOR_DIR m_direction;
	DRV8825_MICROSTEP m_step_size;
	DRV8825_DECAY m_decay;

	/*
	 * We will need many GPIOs there
	 * 1. STEP - saved on Item level (through constructor)
	 *
	 *
	 */
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_reset;  //nRESET - Reset input (pin 16)
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_sleep;  //nSLEEP - Sleep mode input (pin 17)
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_dir;    //DIR - direction input (pin 20)
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_enable; //nENBL - Enable input (pin 21)

	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_dacay; //DECAY - Decay mode (pin 19) not mandatory!

	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_mode_0; //Mode0 - Microstep mode 0 (pin 24)
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_mode_1; //Mode0 - Microstep mode 1 (pin 25)
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_mode_2; //Mode0 - Microstep mode 2 (pin 26)

};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_DRV8825STEPPERMOTOR_H_ */
