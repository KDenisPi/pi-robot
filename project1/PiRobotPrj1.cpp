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
#include "MCP320X.h"
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
    add_provider("SPI", "SPI. One channel");
    

    /*
    * GPIOs
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

    add_gpio("Simple", pirobot::gpio::GPIO_MODE::OUT, 0);
    
    // ****************** Items ****************

    //Analog/Digatal converter MCP320X
    pirobot::item::ItemConfig mcp_3208 = {pirobot::item::ItemTypes::AnlgDgtConvertor, 
        "MCP3208", "MCP3208", 
        {
            std::make_pair("SIMPLE", 0)
        }
    };
    add_item(mcp_3208);

    pirobot::item::ItemConfig light_meter = {pirobot::item::ItemTypes::AnalogMeter, "LightMeter", "LightMeter", {}, "MCP3208", 0};
    add_item(light_meter);

    //BTN Stop
    pirobot::item::ItemConfig btn_stop = {pirobot::item::ItemTypes::BUTTON, "BTN_Stop", "BTN_Stop", {std::make_pair("MCP23017_1", BTN_Stop)}};
    add_item(btn_stop);

    //LEDs Red, Blue, Yellow
    pirobot::item::ItemConfig led_red = {pirobot::item::ItemTypes::LED, "LED_Red", "LED_Red", {std::make_pair("MCP23017_1", LED_Red)}};
    pirobot::item::ItemConfig led_blue = {pirobot::item::ItemTypes::LED, "LED_Blue", "LED_Blue", {std::make_pair("MCP23017_1", LED_Blue)}};
    pirobot::item::ItemConfig led_yellow = {pirobot::item::ItemTypes::LED, "LED_Yellow", "LED_Yellow", {std::make_pair("MCP23017_1", LED_Yellow)}};

    add_item(led_red);
    add_item(led_blue);
    add_item(led_yellow);

    pirobot::item::ItemConfig blinker_red = {pirobot::item::ItemTypes::BLINKER, "BLINK_Red", "BLINK_Red", {}, "LED_Red"};
    pirobot::item::ItemConfig blinker_blue = {pirobot::item::ItemTypes::BLINKER, "BLINK_Blue", "BLINK_Blue", {}, "LED_Blue"};
    pirobot::item::ItemConfig blinker_yellow = {pirobot::item::ItemTypes::BLINKER, "BLINK_Yellow", "BLINK_Yellow", {}, "LED_Yellow"};
    
    add_item(blinker_red);
    add_item(blinker_blue);
    add_item(blinker_yellow);

    // Add DRV 8835
    pirobot::item::ItemConfig drv_8835 = {pirobot::item::ItemTypes::DRV8835, 
        "DRV_8835", "DRV_8835", 
        {
            std::make_pair("MCP23017_1", FAKE_Mode)
        }
    };
    add_item(drv_8835);
    
    //Add DC motor
    pirobot::item::ItemConfig dcm_1 = {pirobot::item::ItemTypes::DCMotor, "DCM_1", "DCM_1", 
            {std::make_pair("MCP23017_1", A_PHASE), std::make_pair("PCA9685", A_ENABLE)}, "DRV_8835"};
    add_item(dcm_1);
                                 
    //Add Stepper ULN2003Stepper
    pirobot::item::ItemConfig step_1 = {pirobot::item::ItemTypes::ULN2003Stepper, "STEP_1", "STEP_1", 
        {
            std::make_pair("MCP23017_1", STEP_PB0), 
            std::make_pair("MCP23017_1", STEP_PB1), 
            std::make_pair("MCP23017_1", STEP_PB2), 
            std::make_pair("MCP23017_1", STEP_PB3)
        }
    };
    add_item(dcm_1);
                        
    //items_add(std::string("SMT_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(34), "SMT_1", "LED 9685 4")));
    //items_add(std::string("SMT_2"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(35), "SMT_2", "LED 9685 5")));

    printConfig();

    logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is finished");
    return true;
}


} /* namespace project1 */
