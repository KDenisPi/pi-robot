/*
 * StInitializeLcd.h
 *
 *  Created on: June 01, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_STINITIALIZE_LCD_H_
#define WEATHER_STINITIALIZE_LCD_H_

#include "StateMachine.h"
#include "defines.h"
namespace weather {

class StInitializeLcd : public smachine::state::State {
public:
    StInitializeLcd() : smachine::state::State("StInitializeLcd") {}
    virtual ~StInitializeLcd(){}

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;
};

}//weather namespace
#endif