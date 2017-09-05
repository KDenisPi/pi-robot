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
#include "ULN2003StepperMotor.h"
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
	add_provider("MCP23017", "MCP23017_1");
	add_provider("PCA9685", "PCA9685");

	/*
	* GPIOs
	*/

	int i;

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

    #define STEP_PB0        8
    #define STEP_PB1        9
    #define STEP_PB2        10
    #define STEP_PB3        11

    #define A_ENABLE        0
    #define B_ENABLE        1


	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, LED_Red);
	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, LED_Blue);
	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, LED_Yellow);

	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, A_PHASE);
	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, B_PHASE);
	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, FAKE_Mode);

	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, STEP_PB0);
	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, STEP_PB1);
	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, STEP_PB2);
	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::OUT, STEP_PB3);

	add_gpio("MCP23017_1", pirobot::gpio::GPIO_MODE::IN, BTN_Stop);

	add_gpio("PCA9685", pirobot::gpio::GPIO_MODE::OUT, A_ENABLE);
	add_gpio("PCA9685", pirobot::gpio::GPIO_MODE::OUT, B_ENABLE);

	// ****************** Items ****************

	//BTN Stop
   	items_add("BTN_Stop", std::shared_ptr<pirobot::item::Item>(new pirobot::item::Button(get_gpio(MCP23017_1+BTN_Stop), 
                 "BTN_Stop", "BTN_Stop", pirobot::item::BUTTON_STATE::BTN_NOT_PUSHED, pirobot::gpio::PULL_MODE::PULL_UP)));
    logger::log(logger::LLOG::NECECCARY, __func__, "Added Item BTN_Stop");

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
                                 get_gpio((std::static_pointer_cast<pirobot::gpio::GpioProvider>(get_provider("PCA9685")))->getStartPin()+A_ENABLE),
                                 "DCM_1",
                                 "DCM_1")));

    items_add("STEP_1", std::shared_ptr<pirobot::item::Item>(new pirobot::item::ULN2003StepperMotor(
                            get_gpio(MCP23017_1+STEP_PB0),
                            get_gpio(MCP23017_1+STEP_PB1),
                            get_gpio(MCP23017_1+STEP_PB2),
                            get_gpio(MCP23017_1+STEP_PB3),
                            "STEP_1", "STEP_1")));

	//items_add(std::string("SMT_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(34), "SMT_1", "LED 9685 4")));
	//items_add(std::string("SMT_2"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(35), "SMT_2", "LED 9685 5")));

    printConfig();

	logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is finished");
	return true;
}


} /* namespace project1 */
