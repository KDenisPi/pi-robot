/*
 * State.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#include "StateMachine.h"
#include "State.h"

namespace smachine {
namespace state {

bool State::init_timer(const int id, const time_t tv_sec, long tv_nsec, bool interval){
    struct smachine::timer::timer_info tm_info = {id, tv_sec, tv_nsec, interval};
    return STM_TIMER_CREATE(tm_info);
}


} /* namespace state */
} /* namespace smachine */
