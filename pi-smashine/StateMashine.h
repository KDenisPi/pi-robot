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
#include <stack>
#include <pthread.h>


#include "State.h"
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

	/*
	 * Add a new event to the queue
	 *
	 * set force to true if need to clear queue and push event
	 */
	void put_event(const std::shared_ptr<Event> event, bool force = false);

private:
	bool start();
	void stop();

	bool is_stopped() { return (m_pthread == 0);}

	static void* worker(void* p);
	pthread_t m_pthread;

	std::recursive_mutex mutex_sm;
	std::queue<std::shared_ptr<Event>> m_events;
	std::stack<std::shared_ptr<State>> m_states;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_STATEMASHINE_H_ */
