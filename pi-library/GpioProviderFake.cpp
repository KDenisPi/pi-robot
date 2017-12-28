/*
 * GpioProviderFake.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#include <iostream>

#include "GpioProviderFake.h"
#include "logger.h"

namespace pirobot{
namespace gpio {

const char TAG[] = "PrvFake";

const int GpioProviderFake::s_pins = 16;

GpioProviderFake::GpioProviderFake(const std::string name, const int pins) :
		GpioProvider(name, pins)
{
}

GpioProviderFake::~GpioProviderFake() {
}

const std::string GpioProviderFake::to_string()
{
	return std::string("GpioFakeProvider");
}

const int GpioProviderFake::dgtRead(const int pin)
{
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" from pin: ") + std::to_string(pin));
	return gpio::SGN_LEVEL::SGN_LOW;
}

void GpioProviderFake::dgtWrite(const int pin, const int value)
{
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" to pin: ") + std::to_string(pin) + " value: " + std::to_string(value));
}

void GpioProviderFake::setmode(const int pin, const GPIO_MODE mode){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" for pin: ") + std::to_string(pin) + " mode: " + std::to_string(mode));
}

void GpioProviderFake::pullUpDownControl(const int pin, const PULL_MODE pumode){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" for pin: ") + std::to_string(pin) + " UP mode: " + std::to_string(pumode));
}

void GpioProviderFake::setPulse(const int pin, const uint16_t pulselen){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" to pin: ") + std::to_string(pin) +
			" pulselen: " + std::to_string(pulselen));
}

}
}
