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

#include "GpioProvider.h"
#include "gpio.h"
#include "item.h"

namespace pirobot {

class PiRobot {
public:
	PiRobot(const bool realWorld = false);
	virtual ~PiRobot();

	/*
	 * Initialize components
	 *
	 * TODO: Read configuration from file
	 */
	virtual bool configure();

	/*
	 *
	 */
	void start();

	/*
	 *
	 */
	void stop();

	/*
	 *
	 */
	std::shared_ptr<gpio::Gpio> get_gpio(const int id) const;

	void gpios_add(int idx, const std::shared_ptr<gpio::Gpio> gpio){
		gpios[idx] = gpio;
	}

	void items_add(const std::string name, const std::shared_ptr<item::Item> item){
		items[name] = item;
	}

        void printConfig();
private:
	bool m_realWorld;

	std::map <int,
		std::shared_ptr<gpio::Gpio>,
		std::less<int>,
		std::allocator<std::pair<const int, std::shared_ptr<gpio::Gpio>> >
	> gpios;

	std::map <std::string,
		std::shared_ptr<item::Item>,
		std::less<std::string>,
		std::allocator<std::pair<const std::string, std::shared_ptr<item::Item>> >
	> items;

};

} /* namespace pirobot */

#endif /* PI_LIBRARY_PIROBOT_H_ */
