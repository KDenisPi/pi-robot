/*
 * GpioProviderSimple.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#include <iostream>
#include <wiringPi.h>

#include "GpioProviderSimple.h"

namespace gpio {

GpioProviderSimple::GpioProviderSimple() {
}

GpioProviderSimple::~GpioProviderSimple() {
}

const std::string GpioProviderSimple::toString()
{
	return std::string("GpioSimpleProvider");
}

const int GpioProviderSimple::dgtRead(int pin)
{
	std::cout << toString() << " digitalRead from " << pin << std::endl;
	return digitalRead(pin);
}

void GpioProviderSimple::dgtWrite(const int pin, const int value)
{
	std::cout << toString() << " digitalWrite to: " << pin <<  " value:" << value << std::endl;
	digitalWrite(pin, value);
}

}
