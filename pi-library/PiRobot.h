/*
 * PiRobot.h
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_PIROBOT_H_
#define PI_LIBRARY_PIROBOT_H_

#include <map>
#include <memory>
#include <wiringPi.h>

#include "gpio.h"
#include "GpioProvider.h"

namespace pirobot {



class PiRobot {
public:
	PiRobot(const bool realWorld = true);
	virtual ~PiRobot();

	/*
	 * Initialize components
	 *
	 * TODO: Read configuration from file
	 */
	virtual bool configure();

private:
	bool m_realWorld;

	std::map <int,
		std::shared_ptr<gpio::Gpio>,
		std::less<int>,
		std::allocator<std::pair<const int, std::shared_ptr<gpio::Gpio>> >
	> gpio;
};

} /* namespace pirobot */

#endif /* PI_LIBRARY_PIROBOT_H_ */
