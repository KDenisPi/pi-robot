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
#include "core_dma.h"
#include "core_clock_pwm.h"
#include "logger.h"

namespace pirobot {
namespace item {
namespace sledctrl {


class SLedCtrlPwm : public pirobot::item::sledctrl::SLedCtrl
{
public:
    SLedCtrlPwm(const int sleds,
        const std::string& name,
        const std::string& comment="") :
            SLedCtrl(item::ItemTypes::SLEDCRTLPWM, sleds, name, comment)
    {
        logger::log(logger::LLOG::DEBUG, "SLedCtrlPwm", std::string(__func__) + " name " + name);
    }

    virtual ~SLedCtrlPwm(){
        logger::log(logger::LLOG::DEBUG, "SLedCtrlPwm", std::string(__func__));
    }

    virtual bool write_data(unsigned char* data, int len) override {
        logger::log(logger::LLOG::DEBUG, "SLedCtrlPwm", std::string(__func__) + " Write to PWM: " +  std::to_string(len));
        return true;
    }

    /*
    * Print device configuration
    */
    virtual const std::string printConfig() override {
        std::string result =  name() + "\n";
        result += SLedCtrl::printConfig();

        return result;
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

    /*
    * Set initial values for PWM
    */
   void _configure() {

   }
};


}
}
}

#endif