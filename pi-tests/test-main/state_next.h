/**
 * @file state_next.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-11-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef TMAIN_STATE_NEXT_H_
#define TMAIN_STATE_NEXT_H_

#include "StateMachine.h"
#include "defines.h"

namespace tmain {

class StNext : public smachine::state::State {
public:
    StNext() : smachine::state::State("StNext") {

    }

    virtual ~StNext(){

    }

    virtual void OnEntry() override {
        logger::log(logger::LLOG::DEBUG, "StNext", std::string(__func__));

        init_timer(TIMER_5, 2, 0, false);
    }

    virtual bool OnTimer(const int id) override {
        logger::log(logger::LLOG::DEBUG, "StNext", std::string(__func__) + " OnTimer ID: " + std::to_string(id));

        switch(id){
            case TIMER_5:
            {
                STM_STATE_POP();
                return true;
            }
        }

        return false;
    }

    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override {
        logger::log(logger::LLOG::DEBUG, "StNext", std::string(__func__));
        return true;
    }

    virtual void OnSubstateExit(const std::string substate_name) override {
        logger::log(logger::LLOG::DEBUG, "StNext", std::string(__func__) + " " + substate_name);
    }
};

}

#endif