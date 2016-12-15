/*
 * StateUpdateState.cpp
 *
 *  Created on: Dec 8, 2016
 *      Author: denis
 */

#include "defines.h"
#include "StateUpdateState.h"

namespace project1 {
namespace state {

StateUpdateState::StateUpdateState(smachine::StateMachineItf* itf) :
		smachine::state::State(itf, "StateUpdateState") {
	// TODO Auto-generated constructor stub

}

StateUpdateState::~StateUpdateState() {
	// TODO Auto-generated destructor stub
}

void StateUpdateState::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEntry started");

	//set finish timer
	get_itf()->timer_start(TIMER_FINISH_ROBOT, 10);
	//POP state
	get_itf()->state_pop();
}

bool StateUpdateState::OnTimer(const int id){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer started ID: " + std::to_string(id));

	return false;
}


} /* namespace state */
} /* namespace project1 */
