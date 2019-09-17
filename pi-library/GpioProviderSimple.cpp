/*
 * GpioProviderSimple.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#include <iostream>

#include "GpioProviderSimple.h"
#include "timers.h"
#include "logger.h"

namespace pirobot {
namespace gpio {

const char TAG[] = "PrvSmpl";

/*
*
*/
GpioProviderSimple::GpioProviderSimple(const std::string name, const int pins) : _gctrl(nullptr),
        GpioProvider(name, pins)
{
    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + std::string(" pins: ") + std::to_string(pins));
    _gctrl = piutils::map_memory<gpio_ctrl>(0, "/dev/gpiomem");
    if(!_gctrl){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + std::string(" Could not map GPIO control registers"));
        throw std::runtime_error(std::string("Could not map GPIO memory"));
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" GPIO control registers mapped"));
}

/*
*
*/
GpioProviderSimple::~GpioProviderSimple() {
    logger::log(logger::LLOG::INFO, TAG, std::string(__func__));

    if(_gctrl){
        piutils::unmap_memory<gpio_ctrl>(static_cast<gpio_ctrl*>((void*)_gctrl));
    }
}

/*
*
*/
const std::string GpioProviderSimple::to_string()
{
    return "Name: " + get_name() + " Type:" + std::string("GpioSimpleProvider");
}

/*
*
*/
const int GpioProviderSimple::dgtRead(const int pin)
{
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" from pin: ") + std::to_string(pin));

    int phpin = phys_pin(pin);
    int idx = (phpin/32); // 32 GPIO by register
    const uint32_t mask = (1 << (phpin%32));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" ph pin: ") + std::to_string(phpin) + " Idx: " + std::to_string(idx) + " Mask: " + std::to_string(mask));

    std::lock_guard<std::mutex> lock(_mx_gpio);
    return ((_gctrl->_GPLEV[idx] & mask) ? 1 : 0);
}

/*
*
*/
void GpioProviderSimple::dgtWrite(const int pin, const int value)
{
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" to pin: ") + std::to_string(pin) + " value: " + std::to_string(value));
    int phpin = phys_pin(pin);
    int idx = (phpin/32); // 32 GPIO by register
    const uint32_t mask = (1 << (phpin%32));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" ph pin: ") + std::to_string(phpin) + " Idx: " + std::to_string(idx) + " Mask: " + std::to_string(mask));

    std::lock_guard<std::mutex> lock(_mx_gpio);
    if(value == 0) //clear value
        _gctrl->_GPCLR[idx] = mask;
    else //set value
        _gctrl->_GPSET[idx] = mask;
}

void GpioProviderSimple::setmode(const int pin, GPIO_MODE mode){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" for pin: ") + std::to_string(pin) + " mode: " + std::to_string(mode));
    int phpin = phys_pin(pin);

    /*
    * For PWM mode detect real mode
    */
    if(mode == GPIO_MODE::PWM_OUT){
        GPIO_MODE pwm_mode = get_pwm_mode(phpin);
        if(pwm_mode == mode){
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + std::string(" We do not support PWM for pin: ") + std::to_string(pin));
            return;
        }
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" PWM mode: ") + std::to_string(pwm_mode));
        mode = pwm_mode;
    }

    int idx = (phpin/9); // 9 GPIO by register
    const uint32_t mask = (mode << (3*(phpin%9)));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" ph pin: ") + std::to_string(phpin) + " Idx: " + std::to_string(idx) + " Mask: " + std::to_string(mask));

    std::lock_guard<std::mutex> lock(_mx_gpio);
    _gctrl->_GPFSEL[idx] |= mask;
}

/*
BCM2835 ARM Peripheral Section 6.1
GPIO Pull-up/down Register (GPPUD) & GPIO Pull-up/down Clock Registers (GPPUDCLKn)

1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither
to remove the current Pull-up/down)
2. Wait 150 cycles – this provides the required set-up time for the control signal
3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
modify – NOTE only the pads which receive a clock will be modified, all others will
retain their previous state.
4. Wait 150 cycles – this provides the required hold time for the control signal
5. Write to GPPUD to remove the control signal
6. Write to GPPUDCLK0/1 to remove the clock
*/
void GpioProviderSimple::pullUpDownControl(const int pin, const PULL_MODE pumode){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" for pin: ") + std::to_string(pin) + " UP mode: " + std::to_string(pumode));

    int phpin = phys_pin(pin);
    int idx = (phpin/32); // 32 GPIO by register
    const uint32_t mask = (1 << (phpin%32));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" ph pin: ") + std::to_string(phpin) + " Idx: " + std::to_string(idx) + " Mask: " + std::to_string(mask));

    std::lock_guard<std::mutex> lock(_mx_gpio);
    _gctrl->_GPPUD = (uint32_t)pumode;          //step 1
    piutils::timers::Timers::delay_micro(10);   //step 2
    _gctrl->_GPPUDCLK[idx] = mask;              //step 3
    piutils::timers::Timers::delay_micro(10);   //step 4
    _gctrl->_GPPUD = 0;                         //step 5
    _gctrl->_GPPUDCLK[idx] = 0;                 //step 6
}

void GpioProviderSimple::setPulse(const int pin, const uint16_t pulselen){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" to pin: ") + std::to_string(pin) + " pulselen: " + std::to_string(pulselen));
    //TODO: Implement pulse for servo
}

}//namespace gpio
}//namespace pirobot
