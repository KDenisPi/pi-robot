/*
 * GpioProviderSimple.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#include <iostream>
#include <wiringPi.h>

#include "GpioProviderSimple.h"
#include "logger.h"

namespace pirobot {
namespace gpio {

const char TAG[] = "PrvSmpl";

GpioProviderSimple::GpioProviderSimple(const std::string name, const int pins) :
        GpioProvider(name, pins)
{
}

GpioProviderSimple::~GpioProviderSimple() {
}

const std::string GpioProviderSimple::to_string()
{
    return "Name: " + get_name() + " Type:" + std::string("GpioSimpleProvider");
}

const int GpioProviderSimple::dgtRead(const int pin)
{
    //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" from pin: ") + std::to_string(pin));
    return digitalRead(getRealPin(pin));
}

void GpioProviderSimple::dgtWrite(const int pin, const int value)
{
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" to pin: ") + std::to_string(pin) + " value: " + std::to_string(value));
    digitalWrite(getRealPin(pin), value);
}

void GpioProviderSimple::setmode(const int pin, const GPIO_MODE mode){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" for pin: ") + std::to_string(pin) + " mode: " + std::to_string(mode));
    pinMode(getRealPin(pin), (mode == GPIO_MODE::IN ? INPUT : OUTPUT));
}

void GpioProviderSimple::pullUpDownControl(const int pin, const PULL_MODE pumode){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" for pin: ") + std::to_string(pin) + " UP mode: " + std::to_string(pumode));
    pullUpDnControl(getRealPin(pin), pumode);
}

void GpioProviderSimple::setPulse(const int pin, const uint16_t pulselen){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" to pin: ") + std::to_string(pin) +
            " pulselen: " + std::to_string(pulselen));
    //TODO: Implement pulse for servo
}


}
}
