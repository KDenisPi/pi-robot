/*
 * EventChangeState.h
 *
 *  Created on: Nov 25, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_EVENTCHANGESTATE_H_
#define PI_SMACHINE_EVENTCHANGESTATE_H_

#include <string>

#include "Event.h"

namespace smachine {

class EventChangeState: public Event {
public:
	EventChangeState(const std::string state_name) :
		Event(EVENT_TYPE::EVT_CHANGE_STATE, state_name)
	{

	}
	virtual ~EventChangeState();
};

} /* namespace smachine */

#endif /* PI_SMACHINE_EVENTCHANGESTATE_H_ */
