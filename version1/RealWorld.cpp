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
#include "GpioProviderMCP23017.h"

#include "led.h"
#include "button.h"
#include "mservo.h"
#include "logger.h"
#include "TiltSwitch.h"
#include "DRV8825StepperMotor.h"
#include "DCMotor.h"

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
	logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is starting..");

	/* 
	* Providers
	*/
	std::shared_ptr<pirobot::gpio::GpioProvider> provider_simple(new pirobot::gpio::GpioProviderSimple());
	std::shared_ptr<pirobot::gpio::GpioProvider> provider_pca9685(new pirobot::gpio::GpioProviderPCA9685(pwm));
	//std::shared_ptr<pirobot::gpio::GpioProvider> provider_mcp23017(new pirobot::gpio::GpioProviderMCP23017());

	/*
	* GPIOs
	*/

	int i;

/*
        for(i=0; i < 8; i++){
                gpios_add(provider_mcp23017->getStartPin()+i,
                                std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_mcp23017->getStartPin()+i,
                                                pirobot::gpio::GPIO_MODE::OUT,  provider_mcp23017)));
        }
*/

/*
	gpios_add(provider_simple->getStartPin(),
			std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_simple->getStartPin(),
					pirobot::gpio::GPIO_MODE::OUT, provider_simple)));

	gpios_add(provider_simple->getStartPin()+1,
			std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_simple->getStartPin()+1,
					pirobot::gpio::GPIO_MODE::OUT, provider_simple)));
*/
    for(i=0; i < 2; i++){

        gpios_add(provider_simple->getStartPin()+i,
                        std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_simple->getStartPin()+i,
                                        pirobot::gpio::GPIO_MODE::OUT, provider_simple)));

              gpios_add(provider_pca9685->getStartPin()+i,
                  std::shared_ptr<pirobot::gpio::Gpio>(new pirobot::gpio::Gpio(provider_pca9685->getStartPin()+i,
                  pirobot::gpio::GPIO_MODE::OUT,  provider_pca9685)));
     }

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
/*
	items_add(std::string("TILT_1"),
			std::shared_ptr<pirobot::item::Item>(
		new pirobot::item::TiltSwitch(get_gpio(provider_simple->getStartPin()), 
				"TILT_1", 
				"TILT SWITCH 1",
				pirobot::item::BUTTON_STATE::BTN_NOT_PUSHED,
				pirobot::gpio::PULL_MODE::PULL_UP)));
*/

/*
	items_add(std::string("STEP_1"),
		std::shared_ptr<pirobot::item::Item>(
			new pirobot::item::DRV8825_StepperMotor(get_gpio(provider_mcp23017->getStartPin()),
				"STEP_1","Stepper 28BYJ-48")));

*/

/*
enum DRV8825_PIN {
        PIN_ENABLE = 0,
        PIN_RESET = 1,
        PIN_SLEEP = 2,
        PIN_DECAY = 3,
        PIN_MODE_0 = 4,
        PIN_MODE_1 = 5,
        PIN_MODE_2 = 6,
        PIN_DIR = 7
};
*/

/*
	// NOTE: We will have DECAY pin opened
	auto stepper1 = dynamic_cast<pirobot::item::DRV8825_StepperMotor*>(get_item("STEP_1").get());
	stepper1->set_gpio(get_gpio(provider_mcp23017->getStartPin()+1), pirobot::item::DRV8825_PIN::PIN_ENABLE);
	stepper1->set_gpio(get_gpio(provider_mcp23017->getStartPin()+2), pirobot::item::DRV8825_PIN::PIN_RESET);
	stepper1->set_gpio(get_gpio(provider_mcp23017->getStartPin()+3), pirobot::item::DRV8825_PIN::PIN_SLEEP);
	stepper1->set_gpio(get_gpio(provider_mcp23017->getStartPin()+4), pirobot::item::DRV8825_PIN::PIN_MODE_0);
	stepper1->set_gpio(get_gpio(provider_mcp23017->getStartPin()+5), pirobot::item::DRV8825_PIN::PIN_MODE_1);
	stepper1->set_gpio(get_gpio(provider_mcp23017->getStartPin()+6), pirobot::item::DRV8825_PIN::PIN_MODE_2);
	stepper1->set_gpio(get_gpio(provider_mcp23017->getStartPin()+7), pirobot::item::DRV8825_PIN::PIN_DIR);
*/

/*
	items_add(std::string("LED_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(30), "LED_1", "LED 9685 1")));
	items_add(std::string("LED_2"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(31), "LED_2", "LED 9685 2")));
	items_add(std::string("LED_3"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::Led(get_gpio(32), "LED_3", "LED 9685 3")));

	items_add(std::string("SMT_1"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(34), "SMT_1", "LED 9685 4")));
	items_add(std::string("SMT_2"), std::shared_ptr<pirobot::item::Item>(new pirobot::item::ServoMotor(get_gpio(35), "SMT_2", "LED 9685 5")));
*/

    /*
     * PCA9685
     * 0 - Servo Motor PWM
     * 1 - DC Motor	PWM
     *
     * Simple
     * 0 - Drv8835 - mode
     * 1 - DC Motor - direction
     */
        logger::log(logger::LLOG::NECECCARY, __func__, " Create STM 1");
	items_add(std::string("SMT_1"), std::shared_ptr<pirobot::item::Item>(
			new pirobot::item::ServoMotor(get_gpio(provider_pca9685->getStartPin()), "SMT_1", "LED 9685 4")));

        logger::log(logger::LLOG::NECECCARY, __func__, " Create Drv8835");
	std::shared_ptr<pirobot::item::Drv8835> drv8835(
		new pirobot::item::Drv8835(get_gpio(provider_simple->getStartPin()), "Drv8835", "Drv8835"));
	items_add(std::string("Drv8835"), drv8835);

        logger::log(logger::LLOG::NECECCARY, __func__, " Create DCM 1");
	items_add(std::string("DCM_1"),
			std::shared_ptr<pirobot::item::Item>(
					new pirobot::item::dcmotor::DCMotor(
							drv8835,
							get_gpio(provider_simple->getStartPin()+1),
							get_gpio(provider_pca9685->getStartPin()+1),
							"DCM_1",
							"DC Motor")
			)
	);



	logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is finished");
	return true;
}

} /* namespace realworld */
