/*
 * StInitializeSensors.h
 * 
 *  Created on: June 01, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_STINITIALIZE_SENSORS_H_
#define WEATHER_STINITIALIZE_SENSORS_H_

#include "StateMachine.h"
#include "defines.h"
namespace weather {

class StInitializeSensors : public smachine::state::State {
public:
    StInitializeSensors(smachine::StateMachineItf* itf) : smachine::state::State(itf, "StInitializeSensors") {}
    virtual ~StInitializeSensors(){}

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;

};

}//weather namespace
#endif