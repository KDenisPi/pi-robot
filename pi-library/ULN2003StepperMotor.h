/*
 * ULN2003StepperMotor.h
 *
 *  Created on: Jan 18, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_ULN2003STEPPERMOTOR_H_
#define PI_LIBRARY_ULN2003STEPPERMOTOR_H_

#include "item.h"

namespace pirobot {
namespace item {

class ULN2003StepperMotor: public Item {
public:
	ULN2003StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_0,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_1,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_2,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_3);

	ULN2003StepperMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_0,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_1,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_2,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_3,
			const std::string name,
			const std::string comment);

	virtual ~ULN2003StepperMotor();

	void step(const int num_steps = 1);
	void set_direction(const MOTOR_DIR direction);

	inline const MOTOR_DIR direction() const {return m_direction;}

	virtual bool initialize() override;
	virtual const std::string to_string() override;
	virtual const std::string printConfig() override;

	virtual void stop() override;
private:

	/*
	 *
	 */
	inline const uint8_t get_next_step(const uint8_t step){
		if(m_direction == MOTOR_DIR::DIR_CLOCKWISE)
			return (step == 7 ? 0 : step+1);

		return (step == 0 ? 7 : step-1);
	}


	uint8_t m_cmd[8]  = {0x08, 0x0C, 0x04, 0x06, 0x02, 0x03, 0x01, 0x01};

	MOTOR_DIR m_direction;
	uint8_t m_step;

	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_1;
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_2;
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_3;

};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_ULN2003STEPPERMOTOR_H_ */
