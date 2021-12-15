/*
 * ststates.h
 *
 *  Created on: December 9, 2021
 *      Author: Denis Kudia
 */

#ifndef TMAIN_STSTATES_H_
#define TMAIN_STSTATES_H_

#include "StateMachine.h"
#include "defines.h"

namespace tmain {

class StInit : public smachine::state::State {
public:
    StInit(const std::shared_ptr<smachine::StateMachineItf>& itf) : smachine::state::State(itf, "StInit") {
    }

    virtual ~StInit(){}

    virtual void OnEntry() override {

    }

    virtual bool OnTimer(const int id) override {

    }

    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override {

    }

    virtual void OnSubstateExit(const std::string substate_name) override {

    }

};

} //end namespace

#endif
