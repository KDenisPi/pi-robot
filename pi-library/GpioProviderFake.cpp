/*
 * GpioProviderFake.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#include <iostream>

#include "GpioProviderFake.h"
#include "logger.h"

namespace pirobot{
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
	logger::log(logger::LLOG::DEBUD, __func__, std::string(" from pin: ") + std::to_string(pin));
	return 0;
}

void GpioProviderFake::dgtWrite(const int pin, const int value)
{
	logger::log(logger::LLOG::DEBUD, __func__, std::string(" to pin: ") + std::to_string(pin) + " value: " + std::to_string(value));
}

void GpioProviderFake::setmode(int pin, GPIO_MODE mode){
	logger::log(logger::LLOG::DEBUD, __func__, std::string(" for pin: ") + std::to_string(pin) + " mode: " + std::to_string(mode));
}

}
}
