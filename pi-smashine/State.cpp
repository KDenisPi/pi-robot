/*
 * State.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#include "State.h"

namespace smashine {
namespace state {

State::State(const std::shared_ptr<StateMashineItf> itf) :
		m_itf(itf)
{

}

State::~State(){

}

} /* namespace state */
} /* namespace smashine */
