/*
 * State.h
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_STATE_H_
#define PI_SMACHINE_STATE_H_

#include <memory>

#include "Event.h"
#include "Timer.h"
#include "StateMachineItf.h"

namespace smachine {
namespace state {

/*
 * Return true from OnEvent and OnTimer if received Event or Timer
 * was processed inside function
 */

class State {
public:
	State(StateMachineItf* itf, const std::string& name);
	virtual ~State();

	virtual void OnEntry() {}
	virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) {return false;};
	virtual void OnExit() {}
	virtual bool OnTimer(const int id) {return false;};
	//called when child state call POP
	virtual void OnSubstateExit(const std::string substate_name) {}

	inline const std::string get_name() { return m_name;}
	inline bool operator==(const State& state){
		return (m_name == state.m_name);
	}

protected:
	std::shared_ptr<pirobot::PiRobot> get_robot() const { 
		if(m_itf){
			return m_itf->get_robot();
		}
		assert(false);
		return std::shared_ptr<pirobot::PiRobot>();
	}


	//Get Item by name
	template<class T> std::shared_ptr<T> get_item(const std::string name){
		return std::static_pointer_cast<T>(get_robot()->get_item(name));
	}

	/*
	* Timers
	*/
	bool timer_create(const int timer_id, const time_t interval){
		if(m_itf){
			return m_itf->timer_start(timer_id, interval);
		}
		assert(false);
		return false;
	}

	bool timer_cancel(const int timer_id){
		if(m_itf){
			return m_itf->timer_cancel(timer_id);
		}
		assert(false);
		return false;
	}

	bool timer_check(const int timer_id){
		if(m_itf){
			return m_itf->timer_check(timer_id);
		}
		assert(false);
		return false;
	}

	/*
	* States
	*/
	void state_change(const std::string& state){
		if(m_itf){
			return m_itf->state_change(state);
		}
		assert(false);
	}

	void state_pop(){
		if(m_itf){
			return m_itf->state_pop();
		}
		assert(false);
	}

	const std::string get_first_state() const {
		if(m_itf){
			return m_itf->get_first_state();
		}
		assert(false);
		return std::string();
	}

	/*
	* Event
	*/
	void event_add(const std::shared_ptr<Event>& event){
		if(m_itf){
			return m_itf->add_event(event);
		}
		assert(false);
	}

	/*
	* Finish
	*/
	void finish(){
		if(m_itf){
			return m_itf->finish();
		}
		assert(false);
	}

	/*
	* Get Environment
	*/
	template<class T> std::shared_ptr<T> get_env(){
		if(m_itf){
			return std::static_pointer_cast<T>(m_itf->get_env());
		}
		assert(false);
		return std::shared_ptr<T>();
	}

private:
	StateMachineItf* m_itf;
	std::string m_name;
};

} /* namespace state */
} /* namespace smachine */

#endif /* PI_SMACHINE_STATE_H_ */
