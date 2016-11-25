/*
 * StateInit.cpp
 *
 *  Created on: Nov 18, 2016
 *      Author: denis
 */

#include "StateInit.h"

namespace smashine {
namespace state {

StateInit::StateInit(const std::shared_ptr<StateMashineItf> itf, const std::shared_ptr<pirobot::PiRobot> robot):
		state::State(itf, robot)
{
	// TODO Auto-generated constructor stub

}

StateInit::~StateInit() {
	// TODO Auto-generated destructor stub
}

void StateInit::OnEntry(){

}

bool StateInit::OnEvent(const std::shared_ptr<Event> event){
	return false;
}

bool StateInit::OnTimer(const int id){
	return false;
}


} /* namespace state */
} /* namespace smashine */