/*
 * RealWorld.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: denis
 */

//#include "logger.h"

#include "Adafruit_PWMServoDriver.h"
#include "GpioProviderFake.h"
#include "GpioProviderSimple.h"
#include "GpioProviderPCA9685.h"
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
        std::shared_ptr<pirobot::gpio::Adafruit_PWMServoDriver> pwm(new pirobot::gpio::Adafruit_PWMServoDriver());
	//logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is starting..");

	//std::shared_ptr<pirobot::gpio::GpioProvider> provider(new pirobot::gpio::GpioProviderSimple());
	std::shared_ptr<pirobot::gpio::GpioProvider> provider(new pirobot::gpio::GpioProviderPCA9685(pwm));

	gpios_add(30, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(30, pirobot::gpio::GPIO_MODE::OUT, provider)));
	gpios_add(31, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(31, pirobot::gpio::GPIO_MODE::OUT, provider)));
	gpios_add(32, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(32, pirobot::gpio::GPIO_MODE::OUT, provider)));

	items_add(std::string("LED_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(30), "LED_1", "LED 9685 1")));
	items_add(std::string("LED_2"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(31), "LED_2", "LED 9685 2")));
	items_add(std::string("LED_3"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(32), "LED_3", "LED 9685 3")));
	//items_add(std::string("BTN_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Button(get_gpio(1), "BTN_1", "Button for LED_1")));

	//logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is finished");
	return true;
}

} /* namespace realworld */
