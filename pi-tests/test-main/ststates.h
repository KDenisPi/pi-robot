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
#include "Context.h"

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
        logger::log(logger::LLOG::DEBUG, "StInit", std::string(__func__));

        const auto ctx = GET_ENV(tmain::Context);
        auto name = ctx->name();
        logger::log(logger::LLOG::DEBUG, "StInit", std::string(__func__) + " Name: " + name);


        init_timer(TIMER_1, 10, 0, false);
        init_timer(TIMER_2, 2, 0, true);
        init_timer(TIMER_3, 1, 0, false);
    }

    virtual bool OnTimer(const int id) override {
        logger::log(logger::LLOG::DEBUG, "StInit", std::string(__func__) + " OnTimer ID: " + std::to_string(id));

        switch(id){
            case TIMER_1:
            {
                STM_FINISH();
                return true;
            }
            case TIMER_2:
            {
                init_timer(TIMER_4, 1, 0, false);
                return true;
            }
            case TIMER_3:
            {
                STM_STATE_CHANGE("StNext")
                return true;
            }
            case TIMER_4:
            {
                return true;
            }
        }

        return false;
    }

    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override {
        logger::log(logger::LLOG::DEBUG, "StInit", std::string(__func__));
        return true;
    }

    virtual void OnSubstateExit(const std::string substate_name) override {
        logger::log(logger::LLOG::DEBUG, "StInit", std::string(__func__) + " " + substate_name);
    }

};

} //end namespace

#endif
