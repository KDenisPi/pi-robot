/*
 * DCMotor.h
 *
 *  Created on: Jan 10, 2017
 *      Author: denis
 */

#ifndef PI_LIBRARY_DCMOTOR_H_
#define PI_LIBRARY_DCMOTOR_H_

#include "item.h"

namespace pirobot {
namespace item {

enum DCMotor_MODE {
	IN_IN = 0,
	PH_EN = 1
};

enum DCMotor_DIR {
	DIR_CLOCKWISE = 0,
	DIR_COUTERCLOCKWISE = 1
};


class DCMotor: public Item {
public:
	DCMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_mode,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_direction,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_pwm,
			DCMotor_MODE mode = DCMotor_MODE::PH_EN);

	DCMotor(const std::shared_ptr<pirobot::gpio::Gpio> gpio_mode,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_direction,
			const std::shared_ptr<pirobot::gpio::Gpio> gpio_pwm,
			const std::string name,
			const std::string comment,
			DCMotor_MODE mode = DCMotor_MODE::PH_EN);

	virtual ~DCMotor();

	inline const DCMotor_MODE get_mode() const {return m_mode;}
	inline const DCMotor_DIR get_direction() const {return m_direction;}

	void set_power_level(const float level = 0.0f);
	void set_direction(const DCMotor_DIR direction);
	void stop();

private:
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_dir;
	std::shared_ptr<pirobot::gpio::Gpio> m_gpio_pwm;

	DCMotor_MODE m_mode;
	DCMotor_DIR m_direction;
};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_DCMOTOR_H_ */
