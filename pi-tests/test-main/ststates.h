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
    StInit() : smachine::state::State("StInit") {
        ////std::cout  "StInit::StInit" << std::endl;
    }

    virtual ~StInit(){
        ////std::cout  "StInit::~StInit" << std::endl;
    }

    virtual void OnEntry() override {
        ////std::cout  "StInit OnEntry()" << std::endl;

        init_timer(TIMER_1, 7, 0, false);
        init_timer(TIMER_2, 3, 0, false);
    }

    virtual bool OnTimer(const int id) override {
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

        ////std::cout  "Timer " << std::to_string(id) << std::endl;

        switch(id){
            case TIMER_1:
            {
                STM_FINISH();
                return true;
            }
            case TIMER_2:
            {
                init_timer(TIMER_3, 3, 0, false);
                return true;
            }
            case TIMER_3:
            {
                return true;
            }
        }

        return false;
    }

    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override {
        return true;
    }

    virtual void OnSubstateExit(const std::string substate_name) override {

    }

};

} //end namespace

#endif
