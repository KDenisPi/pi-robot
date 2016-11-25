/*
 * EventChangeState.h
 *
 *  Created on: Nov 25, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_EVENTCHANGESTATE_H_
#define PI_SMASHINE_EVENTCHANGESTATE_H_

#include <string>

#include "Event.h"

namespace smashine {

class EventChangeState: public Event {
public:
	EventChangeState(const std::string state_name) :
		Event(EVENT_TYPE::EVT_CHANGE_STATE) ,
		m_state_name(state_name)
	{

	}
	virtual ~EventChangeState();

	std::string get_name() const {return m_state_name;}

private:
	std::string m_state_name;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_EVENTCHANGESTATE_H_ */
