/*
 * StInitialization.h
 * 
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_STINITIALIZATION_H_
#define WEATHER_STINITIALIZATION_H_

#include "StateMachine.h"
#include "defines.h"
namespace weather {

class StInitialization : public smachine::state::State {
public:
    StInitialization(smachine::StateMachineItf* itf) : smachine::state::State(itf, "StInitialization") {}
    virtual ~StInitialization(){}

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;
	virtual void OnSubstateExit(const std::string substate_name) override;

};

}//weather namespace
#endif