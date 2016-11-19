/*
 * State.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#include "State.h"

namespace smashine {

State::State(const std::shared_ptr<pirobot::PiRobot> robot) :
		m_robot(robot){

}

State::~State(){

}

} /* namespace smashine */
