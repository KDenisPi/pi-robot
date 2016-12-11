/*
 * State.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#include "State.h"

namespace smachine {
namespace state {

State::State(StateMachineItf* itf, const std::string name) :
		m_itf(itf), m_name(name)
{

}

State::~State(){

}

} /* namespace state */
} /* namespace smachine */
