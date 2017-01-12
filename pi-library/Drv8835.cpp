/*
 * DRV8835.cpp
 *
 *  Created on: Jan 11, 2017
 *      Author: denis
 */

#include <cassert>

#include "Drv8835.h"
#include "logger.h"

const char TAG[] = "DRV8835";

namespace pirobot {
namespace item {

Drv8835::Drv8835(const std::shared_ptr<pirobot::gpio::Gpio> gpio_mode,
		DRV8835_MODE mode) :
			Item(gpio_mode, ItemTypes::DRV8835),
			m_mode(mode)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

	//
	//DRV8835 page 5
	//

	//set mode
	//Logic low selects IN/IN mode
	//Logic high selects PH/EN mode
	get_gpio()->digitalWrite((mode == DRV8835_MODE::IN_IN) ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);

}


Drv8835::Drv8835(const std::shared_ptr<pirobot::gpio::Gpio> gpio_mode,
		const std::string name,
		const std::string comment,
		DRV8835_MODE mode) :
					Item(gpio_mode, name, comment, ItemTypes::DRV8835),
					m_mode(mode)
{
	assert(get_gpio() != NULL);
	assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);

	//
	//DRV8835 page 5
	//

	//set mode
	//Logic low selects IN/IN mode
	//Logic high selects PH/EN mode
	get_gpio()->digitalWrite((mode == DRV8835_MODE::IN_IN) ? gpio::SGN_LEVEL::SGN_LOW : gpio::SGN_LEVEL::SGN_HIGH);
}

Drv8835::~Drv8835() {
	// TODO Auto-generated destructor stub
}

/*
 *
 */
const std::string Drv8835::to_string(){
	return name();
}

const std::string Drv8835::printConfig(){
	std::string conf =  name() + "\n" +
			" GPIO Step: " + get_gpio()->to_string() + "\n";
	conf = conf + " Mode: " + (m_mode==DRV8835_MODE::PH_EN ? "PH_EN" : "IN_IN");
	return conf;
}


} /* namespace item */
} /* namespace pirobot */
