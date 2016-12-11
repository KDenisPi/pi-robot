/*
 * StateEnvAnalize.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: denis
 */

#include "defines.h"
#include "StateEnvAnalize.h"

namespace project1 {
namespace state {

StateEnvAnalize::StateEnvAnalize(const std::shared_ptr<smachine::StateMachineItf> itf) :
		smachine::state::State(itf, "StateEnvAnalize")
{
	// TODO Auto-generated constructor stub

}

StateEnvAnalize::~StateEnvAnalize() {
	// TODO Auto-generated destructor stub
}

void StateEnvAnalize::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEntry started");

	get_itf()->timer_start(TIMER_SWITCH_TO_SECOND, 5);

}

bool StateEnvAnalize::OnTimer(const int id){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer started ID: " + std::to_string(id));

	switch(id){
	case TIMER_SWITCH_TO_SECOND:
		/*
		 * Switch to another state
		 */
		get_itf()->state_change("StateUpdateState");
		return true;
	}

	return false;
}


} /* namespace state */
} /* namespace project1 */
