/*
 * RealWorld.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: denis
 */

//#include "logger.h"

#include "GpioProviderFake.h"
#include "GpioProviderSimple.h"
#include "item.h"
#include "led.h"
#include "button.h"

#include "RealWorld.h"


namespace realworld {

RealWorld::RealWorld() :
		pirobot::PiRobot(true)
{

}

/*
 * GPIO 0 - 17 
 * GPIO 1 - 18  
 */
bool RealWorld::configure(){
	//logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is starting..");

	std::shared_ptr<pirobot::gpio::GpioProvider> provider(new pirobot::gpio::GpioProviderSimple());

	gpios_add(0, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(0, pirobot::gpio::GPIO_MODE::OUT, provider)));
	gpios_add(1, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(1, pirobot::gpio::GPIO_MODE::IN, provider)));

	items_add(std::string("LED_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(0), "LED_1", "LED first")));
	items_add(std::string("BTN_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Button(get_gpio(1), "BTN_1", "Button for LED_1")));

	//logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is finished");
	return true;
}

} /* namespace realworld */
