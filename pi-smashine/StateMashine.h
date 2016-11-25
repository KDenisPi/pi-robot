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
#include <list>
#include <pthread.h>

#include "StateMashineItf.h"
#include "PiRobot.h"
#include "StateFactory.h"
#include "State.h"
#include "Event.h"

namespace smashine {

class StateMashine : public StateMashineItf {
public:
	StateMashine(const std::shared_ptr<StateFactory> factory, const std::shared_ptr<pirobot::PiRobot> pirobot);
	virtual ~StateMashine();

    /*
    * Get next event to the event's queue
    */
	const std::shared_ptr<Event> get_event();

	/*
	 * Add a new event to the queue
	 *
	 * set force to true if need to clear queue and push event
	 */
	void put_event(const std::shared_ptr<Event> event, bool force = false);

	/*
	 *
	 */
	virtual void state_change(const std::string new_state) override;
	virtual void state_pop() override;
	virtual void timer_start(const int timer_id, const int interval) override;
	virtual void timer_stop(const int timer_id) override;

private:
	bool start();
	void stop();

	bool is_stopped() const { return (m_pthread == 0);}

	static void* worker(void* p);
	pthread_t m_pthread;

	bool empty() const { return m_events.empty(); }
	std::queue<std::shared_ptr<Event>> get_events() const {return m_events;}
	std::list<std::shared_ptr<state::State>> get_states() const {return m_states;}

	void state_push(const std::shared_ptr<state::State> state);

	std::recursive_mutex mutex_sm;
	std::queue<std::shared_ptr<Event>> m_events;
	std::list<std::shared_ptr<state::State>> m_states;

	std::shared_ptr<pirobot::PiRobot> m_pirobo;
	std::shared_ptr<StateFactory> m_factory;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_STATEMASHINE_H_ */
