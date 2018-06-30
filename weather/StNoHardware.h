/*
 * StNoHardware.h
 *
 * @brief State used for test purposes
 *
 *  Created on: June 29, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_STNOHARDWARE_H_
#define WEATHER_STNOHARDWARE_H_

#include "StateMachine.h"
#include "defines.h"
namespace weather {

class StNoHardware : public smachine::state::State {
public:
    StNoHardware(smachine::StateMachineItf* itf) : smachine::state::State(itf, "StNoHardware") {}
    virtual ~StNoHardware(){}

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;
};

}//weather namespace
#endif