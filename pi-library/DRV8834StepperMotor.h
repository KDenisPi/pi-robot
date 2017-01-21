/*
 * DRV8834StepperMotor.h
 *
 *  Created on: Jan 20, 2017
 *      Author: denis
 */

#ifndef PI_LIBRARY_DRV8834STEPPERMOTOR_H_
#define PI_LIBRARY_DRV8834STEPPERMOTOR_H_

#include "item.h"
#include "Drv8825_8834.h"

namespace pirobot {
namespace item {

class DRV8834_StepperMotor: public Item {
public:
	DRV8834_StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio);

	DRV8834_StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment);

	virtual ~DRV8834_StepperMotor();

	void set_gpio(const std::shared_ptr<pirobot::gpio::Gpio> gpio, const DRV88_PIN pin);
	void set_direction(const MOTOR_DIR dir);
	void set_enable(const bool enable);
	void set_sleep(const bool sleep);

	void set_step_size(const DRV8834_MICROSTEP step_size);
	void step(const int num_steps = 1);

	inline const bool is_enable() const {return m_enable;}
	inline const bool is_sleep() const {return m_sleep;}

	inline const MOTOR_DIR direction() const {return m_direction;}
	inline const DRV8834_MICROSTEP step_size() const {return m_step_size;}

	virtual bool initialize() override;
	virtual const std::string to_string() override;
	virtual const std::string printConfig() override;

private:

	bool m_enable;
	bool m_sleep;

	MOTOR_DIR m_direction;
	DRV8834_MICROSTEP m_step_size;

	/*
	 * We will need many GPIOs there
	 * 1. STEP - saved on Item level (through constructor)
	 *
	 *
	 */
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_sleep;  //nSLEEP - Sleep mode input (pin 17)
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_dir;    //DIR - direction input (pin 20)
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_enable; //nENBL - Enable input (pin 21)

	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_mode_0; //Mode0 - Microstep mode 0 (pin 24)
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_mode_1; //Mode0 - Microstep mode 1 (pin 25)

};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_DRV8834STEPPERMOTOR_H_ */
