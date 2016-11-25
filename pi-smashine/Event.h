/*
 * Event.h
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_EVENT_H_
#define PI_SMASHINE_EVENT_H_

#include <memory>

namespace smashine {

enum EVENT_TYPE {
   EVT_NONE         = 0,  //Do nothing
   EVT_CHANGE_STATE = 1,  //Change state
   EVT_POP_STATE    = 2,  //Move to previous state in stack
   EVT_TIMER        = 3,  //Timer
   EVT_FINISH       = 4
};   

class Event {
public:
	Event(const EVENT_TYPE type);
	virtual ~Event();

    const EVENT_TYPE type() { return m_type; }

private:
       EVENT_TYPE m_type;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_EVENT_H_ */
