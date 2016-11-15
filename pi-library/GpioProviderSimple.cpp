/*
 * GpioProviderSimple.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: denis
 */

#include <iostream>
#include <wiringPi.h>

#include "GpioProviderSimple.h"
#include "logger.h"

namespace pirobot {
namespace gpio {

GpioProviderSimple::GpioProviderSimple() {
}

GpioProviderSimple::~GpioProviderSimple() {
}

const std::string GpioProviderSimple::toString()
{
	return std::string("GpioSimpleProvider");
}

const int GpioProviderSimple::dgtRead(const int pin)
{
	//logger::log(logger::LLOG::DEBUD, __func__, std::string(" from pin: ") + std::to_string(pin));
	return digitalRead(getRealPin(pin));
}

void GpioProviderSimple::dgtWrite(const int pin, const int value)
{
	logger::log(logger::LLOG::DEBUD, __func__, std::string(" to pin: ") + std::to_string(pin) + " value: " + std::to_string(value));
	digitalWrite(getRealPin(pin), value);
}

void GpioProviderSimple::setmode(const int pin, const GPIO_MODE mode){
	logger::log(logger::LLOG::DEBUD, __func__, std::string(" for pin: ") + std::to_string(pin) + " mode: " + std::to_string(mode));
	pinMode(getRealPin(pin), (mode == GPIO_MODE::IN ? INPUT : OUTPUT));
}

void GpioProviderSimple::pullUpDownControl(const int pin, const PULL_MODE pumode){
	logger::log(logger::LLOG::DEBUD, __func__, std::string(" for pin: ") + std::to_string(pin) + " UP mode: " + std::to_string(pumode));
	pullUpDnControl(getRealPin(pin), pumode);
}


}
}
