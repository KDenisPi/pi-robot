/*
 * State.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#include "State.h"

namespace smashine {
namespace state {

State::State(const std::shared_ptr<StateMashineItf> itf, const std::shared_ptr<pirobot::PiRobot> robot) :
		m_itf(itf), m_robot(robot){

}

State::~State(){

}

} /* namespace state */
} /* namespace smashine */
