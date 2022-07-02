/*
 * State.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#include "State.h"

namespace smachine {
namespace state {

State::State(StateMachineItf* itf, const std::string& name) : m_name(name), m_itf(itf)
{
	std::cout <<  "State " << " " << m_name  << std::endl;
}

State::~State(){
	std::cout <<  "~State " << m_name << std::endl;
}

} /* namespace state */
} /* namespace smachine */
