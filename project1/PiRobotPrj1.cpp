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
#include "blinking.h"
#include "Drv8835.h"
#include "DCMotor.h"
//#include "TiltSwitch.h"
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
	std::shared_ptr<pirobot::gpio::GpioProvider> provider_mcp23017(new pirobot::gpio::GpioProviderMCP23017());
	//PCA9585
        std::shared_ptr<pirobot::gpio::Adafruit_PWMServoDriver> pwm(new pirobot::gpio::Adafruit_PWMServoDriver());
	std::shared_ptr<pirobot::gpio::GpioProvider> provider_pca9685(new pirobot::gpio::GpioProviderPCA9685(pwm));

	/*
	* GPIOs
	*/

	int i;

/*
	for(i=0; i < 3; i++){
		gpios_add(provider_simple->getStartPin()+i,
				std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_simple->getStartPin()+i,
						(i==2 ? pirobot::gpio::GPIO_MODE::OUT : pirobot::gpio::GPIO_MODE::IN),
						provider_simple)));
	}
*/

/*
        gpios_add(provider_simple->getStartPin(),
                                std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_simple->getStartPin(),
                                                pirobot::gpio::GPIO_MODE::OUT, provider_simple)));
*/
	/*
		MCP23017 (1) address 0x20, start pin 50

		pin 9  - LED Red
		pin 10 - BTN Stop
		pin 11 - B Phase for DRV8835
		pin 13 - A Phase for DRV8835
		pin 14 - LED Blue
		pin 15 - LED Yellow
	*/
	#define MCP23017_1	50
	#define LED_Red 	1
	#define BTN_Stop 	2
	#define B_PHASE 	3
	#define A_PHASE 	5
	#define LED_Blue 	6
	#define LED_Yellow	7
        #define FAKE_Mode       15

        #define A_ENABLE        0
        #define B_ENABLE        1

	// LED Red
	gpios_add(provider_mcp23017->getStartPin()+LED_Red,
				std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_mcp23017->getStartPin()+LED_Red,
						pirobot::gpio::GPIO_MODE::OUT, provider_mcp23017)));
	// LED Blue
	gpios_add(provider_mcp23017->getStartPin()+LED_Blue,
				std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_mcp23017->getStartPin()+LED_Blue,
						pirobot::gpio::GPIO_MODE::OUT, provider_mcp23017)));
	// LED Yellow
	gpios_add(provider_mcp23017->getStartPin()+LED_Yellow,
				std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_mcp23017->getStartPin()+LED_Yellow,
						pirobot::gpio::GPIO_MODE::OUT, provider_mcp23017)));
        // A_PHASE
        gpios_add(provider_mcp23017->getStartPin()+A_PHASE,
                                std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_mcp23017->getStartPin()+A_PHASE,
                                                pirobot::gpio::GPIO_MODE::OUT, provider_mcp23017)));
        // B_PHASE
        gpios_add(provider_mcp23017->getStartPin()+B_PHASE,
                                std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_mcp23017->getStartPin()+B_PHASE,
                                                pirobot::gpio::GPIO_MODE::OUT, provider_mcp23017)));
        // FAKE_Mode
        gpios_add(provider_mcp23017->getStartPin()+FAKE_Mode,
                                std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_mcp23017->getStartPin()+FAKE_Mode,
                                                pirobot::gpio::GPIO_MODE::OUT, provider_mcp23017)));
	// BTN Stop
	gpios_add(provider_mcp23017->getStartPin()+BTN_Stop,
				std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_mcp23017->getStartPin()+BTN_Stop,
						pirobot::gpio::GPIO_MODE::IN, provider_mcp23017)));
       // PWM A_ENABLE
        gpios_add(provider_pca9685->getStartPin()+A_ENABLE,
                                std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_pca9685->getStartPin()+A_ENABLE,
                                                pirobot::gpio::GPIO_MODE::OUT, provider_pca9685)));
      // PWM B_ENABLE
        gpios_add(provider_pca9685->getStartPin()+B_ENABLE,
                                std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_pca9685->getStartPin()+B_ENABLE,
                                                pirobot::gpio::GPIO_MODE::OUT, provider_pca9685)));


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

	//BTN Stop
   	items_add("BTN_Stop", std::shared_ptr<pirobot::item::Item>(new pirobot::item::Button(get_gpio(MCP23017_1+BTN_Stop), 
                 "BTN_Stop", "BTN_Stop", pirobot::item::BUTTON_STATE::BTN_NOT_PUSHED, pirobot::gpio::PULL_MODE::PULL_UP)));
        logger::log(logger::LLOG::NECECCARY, __func__, "Added Item BTN_Stop");

        //items_add("LED_White", std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(provider_simple->getStartPin()), "LED_White", "LED_White")));
        //logger::log(logger::LLOG::NECECCARY, __func__, "Added Item LED_White");

	//LEDs Red, Blue, Yellow
 	items_add("LED_Red", std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(MCP23017_1+LED_Red), "LED_Red", "LED_Red")));
 	logger::log(logger::LLOG::NECECCARY, __func__, "Added Item LED_Red");
 	items_add("LED_Blue", std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(MCP23017_1+LED_Blue), "LED_Blue", "LED_Blue")));
 	logger::log(logger::LLOG::NECECCARY, __func__, "Added Item LED_Blue");
 	items_add("LED_Yellow", std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(MCP23017_1+LED_Yellow), "LED_Yellow", "LED_Yellow")));
 	logger::log(logger::LLOG::NECECCARY, __func__, "Added Item LED_Yellow");

 	items_add("BLINK_Red", std::shared_ptr<pirobot::item::Item>(
               new pirobot::item::Blinking<pirobot::item::Led>(std::static_pointer_cast<pirobot::item::Led>(get_item("LED_Red")),"BLINK_Red")));

 	items_add("BLINK_Blue", std::shared_ptr<pirobot::item::Item>(
		 new pirobot::item::Blinking<pirobot::item::Led>(std::static_pointer_cast<pirobot::item::Led>(get_item("LED_Blue")),"BLINK_Blue")));

 	items_add("BLINK_Yellow", std::shared_ptr<pirobot::item::Item>(
		 new pirobot::item::Blinking<pirobot::item::Led>(std::static_pointer_cast<pirobot::item::Led>(get_item("LED_Yellow")),"BLINK_Yellow","BLNK_Yellow")));

        // Add DRV 8835
        items_add("DRV_8835", std::shared_ptr<pirobot::item::Item>(new pirobot::item::Drv8835(get_gpio(MCP23017_1+FAKE_Mode), "DRV_8835", "DRV_8835")));
        //Add DC motor

        items_add("DCM_1", std::shared_ptr<pirobot::item::Item>(new pirobot::item::dcmotor::DCMotor(
                                 std::static_pointer_cast<pirobot::item::Drv8835>(get_item("DRV_8835")),
                                 get_gpio(MCP23017_1+A_PHASE),
                                 get_gpio(provider_pca9685->getStartPin()+A_ENABLE),
                                 "DCM_1",
                                 "DCM_1")));

	//items_add(std::string("SMT_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(34), "SMT_1", "LED 9685 4")));
	//items_add(std::string("SMT_2"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(35), "SMT_2", "LED 9685 5")));

        printConfig();

	logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is finished");
	return true;
}


} /* namespace project1 */
