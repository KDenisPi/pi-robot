/*
 * GpioProviderFake.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#include <iostream>
#include "GpioProviderFake.h"

namespace gpio {

GpioProviderFake::GpioProviderFake() {
}

GpioProviderFake::~GpioProviderFake() {
}

const std::string GpioProviderFake::toString()
{
	return std::string("GpioFakeProvider");
}

const int GpioProviderFake::dgtRead(int pin)
{
	std::cout << toString() << " digitalRead from " << pin << std::endl;
	return 0;
}

void GpioProviderFake::dgtWrite(const int pin, const int value)
{
	std::cout << toString() << " digitalWrite to: " << pin <<  " value:" << value << std::endl;
}

}
