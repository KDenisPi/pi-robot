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

#include "led.h"
#include "button.h"
#include "mservo.h"
#include "logger.h"

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

	//std::shared_ptr<pirobot::gpio::Adafruit_PWMServoDriver> pwm(new pirobot::gpio::Adafruit_PWMServoDriver());
	logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is starting..");

	/* 
	* Providers
	*/
	std::shared_ptr<pirobot::gpio::GpioProvider> provider_simple(new pirobot::gpio::GpioProviderSimple());
	//std::shared_ptr<pirobot::gpio::GpioProvider> provider(new pirobot::gpio::GpioProviderPCA9685(pwm));

	/*
	* GPIOs
	*/
	//gpios_add(30, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(30, pirobot::gpio::GPIO_MODE::OUT, provider_simple)));
	gpios_add(provider_simple->getStartPin(),
			std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_simple->getStartPin(),
					pirobot::gpio::GPIO_MODE::IN, provider_simple)));

	/*
	gpios_add(30, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(30, pirobot::gpio::GPIO_MODE::OUT, provider)));
	gpios_add(31, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(31, pirobot::gpio::GPIO_MODE::OUT, provider)));
	gpios_add(32, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(32, pirobot::gpio::GPIO_MODE::OUT, provider)));

	gpios_add(34, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(34, pirobot::gpio::GPIO_MODE::OUT, provider)));
	gpios_add(35, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(35, pirobot::gpio::GPIO_MODE::OUT, provider)));
*/
	/*
	* Items
	*/
	items_add(std::string("TILT_1"),
			std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(provider_simple->getStartPin()), "TILT_1", "TILT SWITCH 1")));

/*
	items_add(std::string("LED_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(30), "LED_1", "LED 9685 1")));
	items_add(std::string("LED_2"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(31), "LED_2", "LED 9685 2")));
	items_add(std::string("LED_3"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(32), "LED_3", "LED 9685 3")));

	items_add(std::string("SMT_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(34), "SMT_1", "LED 9685 4")));
	items_add(std::string("SMT_2"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(35), "SMT_2", "LED 9685 5")));
*/

	logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is finished");
	return true;
}

} /* namespace realworld */
