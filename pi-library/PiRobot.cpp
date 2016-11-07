/*
 * PiRobot.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#include "PiRobot.h"
#include "GpioProviderFake.h"

namespace pirobot {

PiRobot::PiRobot(const bool realWorld)
	: m_realWorld(realWorld)
{

	if(realWorld) //Under PI
	{
		int wiPi = wiringPiSetup();
	}
}

PiRobot::~PiRobot() {
	/*
	 * Delete all objects
	 */

}


} /* namespace pirobot */
