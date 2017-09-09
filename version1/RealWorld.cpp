/*
 * RealWorld.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: Denis Kudia
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
#include "DRV8834StepperMotor.h"
#include "DCMotor.h"
#include "ULN2003StepperMotor.h"

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

	logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is starting..");


	logger::log(logger::LLOG::NECECCARY, __func__, "Real Robot configuration is finished");
	return true;
}

} /* namespace realworld */
