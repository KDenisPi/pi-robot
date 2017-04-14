/*
 * DCMotor.h
 *
 *  Created on: Jan 10, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_DCMOTOR_H_
#define PI_LIBRARY_DCMOTOR_H_

#include "Drv8835.h"

namespace pirobot {
namespace item {
namespace dcmotor {

class DCMotor: public Item {
public:
	DCMotor(const std::shared_ptr<pirobot::item::Drv8835> drv8835,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_direction,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_pwm,
			MOTOR_DIR direction = MOTOR_DIR::DIR_CLOCKWISE);

	DCMotor(const std::shared_ptr<pirobot::item::Drv8835> drv8835,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_direction,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_pwm,
			const std::string name,
			const std::string comment,
			MOTOR_DIR direction = MOTOR_DIR::DIR_CLOCKWISE);

	virtual ~DCMotor();

	inline const MOTOR_DIR get_direction() const {return m_direction;}

	void set_power_level(const float level = 0.0f);
	void set_direction(const MOTOR_DIR direction);
	void stop();

	virtual const std::string to_string() override;
	virtual const std::string printConfig() override;

private:
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_pwm;
	std::shared_ptr<pirobot::item::Drv8835> m_drv8835;

	MOTOR_DIR m_direction;
};

} /* namespace dcmotor */
} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_DCMOTOR_H_ */
