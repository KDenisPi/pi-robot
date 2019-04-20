/*
 * sledctrl_pwm.h
 *
 * PWM based LED Stripe Controller
 *
 *  Created on: Jan 15, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SLED_CTRL_PWM_H_
#define PI_LIBRARY_SLED_CTRL_PWM_H_

#include "sledctrl.h"
#include "smallthings.h"
#include "core_pwm.h"
#include "logger.h"

namespace pirobot {
namespace item {
namespace sledctrl {


class SLedCtrlPwm : public pirobot::item::sledctrl::SLedCtrl, public pi_core::core_pwm::PwmCore
{
public:
    SLedCtrlPwm(const int sleds,
        const std::string& name,
        const std::shared_ptr<pirobot::gpio::Gpio> gpio,
        const std::string& comment="") :
            SLedCtrl(item::ItemTypes::SLEDCRTLPWM, sleds, name, gpio, comment),  pi_core::core_pwm::PwmCore(10)
    {
        logger::log(logger::LLOG::DEBUG, "SLedCtrlPwm", std::string(__func__) + " name " + name);
    }

    virtual ~SLedCtrlPwm(){
        logger::log(logger::LLOG::DEBUG, "SLedCtrlPwm", std::string(__func__));
    }

    /*
    * Print device configuration
    */
    virtual const std::string printConfig() override {
        std::string result =  name() + "\n";
        result += SLedCtrl::printConfig();

        return result;
    }

    virtual bool write_data(uint8_t* data, int len) override{

        return true;
    }


    /*
    * ON for PWM based device
    */
    virtual void On() override{

    }

    /*
    * OFF for PWM based device
    */
    virtual void Off() override{

    }

};


}
}
}

#endif