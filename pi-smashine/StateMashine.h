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
#include "Timers.h"

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
	// Generate finish signal
	virtual void finish() override;
	virtual void state_change(const std::string new_state) override;
	virtual void state_pop() override;
	virtual void timer_start(const int timer_id, const time_t interval, const bool interval_timer) override;
	virtual void timer_cancel(const int timer_id) override;
	virtual const std::shared_ptr<TimersItf> get_timersitf() override { return std::shared_ptr<TimersItf>(dynamic_cast<TimersItf*>(&m_timers));}
	virtual std::shared_ptr<pirobot::PiRobot> get_robot() override {return m_pirobo;}


	bool process_timer_event(const std::shared_ptr<Event> event);
	void process_pop_state();
	void process_change_state(const std::shared_ptr<Event> event);

	/*
	 * Temporal - return thread ID
	 */
	inline const pthread_t get_thread() const {return m_pthread;}

	/*
	 * Debug function print current stack of states
	 */
	const std::string print_state_stack() const;
private:
	bool start();
	void stop();

	bool is_stopped() const { return (m_pthread == 0);}

	static void* worker(void* p);
	pthread_t m_pthread;

	bool empty() const {return m_events.empty();}

	inline std::queue<std::shared_ptr<Event>> get_events() const {return m_events;}
	inline std::list<std::shared_ptr<state::State>> get_states() const {return m_states;}

	void state_push(const std::shared_ptr<state::State> state);

	std::recursive_mutex mutex_sm;
	std::queue<std::shared_ptr<Event>> m_events;
	std::list<std::shared_ptr<state::State>> m_states;

	/*
	 *
	 */
	smashine::Timers m_timers;

	std::shared_ptr<pirobot::PiRobot> m_pirobo;
	std::shared_ptr<StateFactory> m_factory;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_STATEMASHINE_H_ */
