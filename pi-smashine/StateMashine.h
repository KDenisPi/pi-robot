/*
 * StateMashine.h
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_STATEMASHINE_H_
#define PI_SMASHINE_STATEMASHINE_H_

#include <queue>
#include <memory>
#include <mutex>

#include "Event.h"

namespace smashine {

class StateMashine {
public:
	StateMashine();
	virtual ~StateMashine();

        /*
        * Get next event to the event's quieue
        */  
	const std::shared_ptr<Event> get_event();

private:
	std::recursive_mutex mutex_sm;
	std::queue<std::shared_ptr<Event>> m_events;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_STATEMASHINE_H_ */
