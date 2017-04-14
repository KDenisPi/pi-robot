/*
 * PiRobotPrj1.cpp
 *
 *  Created on: Dec 17, 2016
 *      Author: Denis Kudia
 */

#include <vector>

#include "PiRobotPrj1.h"

#include "GpioProviderSimple.h"
#include "GpioProviderMCP23017.h"

#include "Adafruit_PWMServoDriver.h"
#include "GpioProviderPCA9685.h"

//#include "GpioProviderFake.h"

#include "led.h"
#include "button.h"
#include "TiltSwitch.h"

//#include "mservo.h"

#include "logger.h"

namespace project1 {

PiRobotPrj1::PiRobotPrj1() : pirobot::PiRobot(true) {
	// TODO Auto-generated constructor stub

}

PiRobotPrj1::~PiRobotPrj1() {
	// TODO Auto-generated destructor stub
}

/*
 * Build In
 * GPIO 0 - 17
 * GPIO 1 - 18
 */
bool PiRobotPrj1::configure(){

	/*
	* Providers
	*/

    //build in GPIO
	std::shared_ptr<pirobot::gpio::GpioProvider> provider_simple(new pirobot::gpio::GpioProviderSimple());
	//GPIO extender MCP23017
	//std::shared_ptr<pirobot::gpio::GpioProvider> provider_mcp23017(new pirobot::gpio::GpioProviderMCP23017());
	//PCA9585
    //std::shared_ptr<pirobot::gpio::Adafruit_PWMServoDriver> pwm(new pirobot::gpio::Adafruit_PWMServoDriver());
	//std::shared_ptr<pirobot::gpio::GpioProvider> provider_pca9685(new pirobot::gpio::GpioProviderPCA9685(pwm));

	/*
	* GPIOs
	*/

	int i;
	for(i=0; i < 3; i++){
		gpios_add(provider_simple->getStartPin()+i,
				std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_simple->getStartPin()+i,
						(i==2 ? pirobot::gpio::GPIO_MODE::OUT : pirobot::gpio::GPIO_MODE::IN),
						provider_simple)));
	}
/*
	for(i=0; i < 2; i++){
		gpios_add(provider_mcp23017->getStartPin()+i,
				std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_mcp23017->getStartPin()+i,
						pirobot::gpio::GPIO_MODE::OUT,	provider_mcp23017)));
	}

	for(i=0; i < 3; i++){
		gpios_add(provider_pca9685->getStartPin()+i,
				std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_pca9685->getStartPin()+i,
						pirobot::gpio::GPIO_MODE::OUT, provider_pca9685)));
	}

*/
	// Items
	items_add("TILT_1", std::shared_ptr<pirobot::item::Item>(new pirobot::item::TiltSwitch(get_gpio(0), "TILT_1", "TILT_1")));
	        logger::log(logger::LLOG::NECECCARY, __func__, "Added Item TILT_1");
   	items_add("BTN_1", std::shared_ptr<pirobot::item::Item>(new pirobot::item::Button(get_gpio(1), "BTN_1", "BTN_1")));
   	        logger::log(logger::LLOG::NECECCARY, __func__, "Added Item BTN_1");
 	items_add("LED_1", std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(2), "LED_1", "LED_1")));
 	        logger::log(logger::LLOG::NECECCARY, __func__, "Added Item LED_1");

/*
	std::vector<int> gpios = get_gpios();
	for(int gpio : gpios){
		std::string name = std::string("LED_") + std::to_string(gpio);
		items_add(name, std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(gpio), name, name)));
  	        logger::log(logger::LLOG::NECECCARY, __func__, "Added Item " + name);
	}

        for(i=2; i < 4; i++){
		int pin = provider_mcp23017->getStartPin()+i;
                gpios_add(pin, std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(pin, pirobot::gpio::GPIO_MODE::IN, 
			provider_mcp23017)));

                std::string name = std::string("BTN_") + std::to_string(pin);
                items_add(name, std::shared_ptr<pirobot::item::Item>(new pirobot::item::Button(get_gpio(pin), name, name)));
                logger::log(logger::LLOG::NECECCARY, __func__, "Added Item " + name);
        }
*/
	//items_add(std::string("SMT_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(34), "SMT_1", "LED 9685 4")));
	//items_add(std::string("SMT_2"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(35), "SMT_2", "LED 9685 5")));

        printConfig();

	logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is finished");
	return true;
}


} /* namespace project1 */
