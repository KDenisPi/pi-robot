/*
 * StateMashine.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#include "StateMashine.h"

namespace smashine {

StateMashine::StateMashine() {
	// TODO Auto-generated constructor stub

}

StateMashine::~StateMashine() {
	// TODO Auto-generated destructor stub
}

/*
 *
 */
const std::shared_ptr<Event> StateMashine::get_event(){
	mutex_sm.lock();
	std::shared_ptr<Event> event = m_events.front();
	m_events.pop();
	mutex_sm.unlock();
	return event;
}


} /* namespace smashine */
