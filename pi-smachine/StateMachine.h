/*
 * StateMachine.h
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#ifndef PI_SMACHINE_STATEMACHINE_H_
#define PI_SMACHINE_STATEMACHINE_H_

#include <queue>
#include <memory>
#include <mutex>
#include <list>
#include <pthread.h>

#include "StateMachineItf.h"
#include "PiRobot.h"
#include "StateFactory.h"
#include "State.h"
#include "EventChangeState.h"
#include "Event.h"
//#include "Timers.h"

namespace smachine {

class Timers;

class StateMachine : public StateMachineItf {
public:
	StateMachine(const std::shared_ptr<StateFactory> factory, const std::shared_ptr<pirobot::PiRobot> pirobot);
	virtual ~StateMachine();

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

	virtual std::shared_ptr<pirobot::PiRobot> get_robot() override {return m_pirobo;}


	bool process_timer_event(const std::shared_ptr<Event> event);
	void process_pop_state();
	void process_change_state(const std::shared_ptr<Event> event);
	void process_finish_event();

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

	inline std::shared_ptr<std::list<std::shared_ptr<state::State>>> get_states() const {return m_states;}

	void state_push(const std::shared_ptr<state::State> state);

	std::recursive_mutex mutex_sm;
	std::queue<std::shared_ptr<Event>> m_events;
	std::shared_ptr<std::list<std::shared_ptr<state::State>>> m_states;

	/*
	 *
	 */
	std::shared_ptr<smachine::Timers> m_timers;

	std::shared_ptr<pirobot::PiRobot> m_pirobo;
	std::shared_ptr<StateFactory> m_factory;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_STATEMACHINE_H_ */
