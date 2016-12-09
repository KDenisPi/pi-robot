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

StateUpdateState::StateUpdateState() {
	// TODO Auto-generated constructor stub

}

StateUpdateState::~StateUpdateState() {
	// TODO Auto-generated destructor stub
}

void StateUpdateState::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEntry started");

	get_itf()->timer_start(TIMER_FINISH_ROBOT, 4);

}

bool StateUpdateState::OnTimer(const int id){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer started ID: " + std::to_string(id));

	switch(id){
	case TIMER_FINISH_ROBOT:
		get_itf()->finish();
		return true;
	}

	return false;
}


} /* namespace state */
} /* namespace project1 */
