/*
 * State.h
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_STATE_H_
#define PI_SMACHINE_STATE_H_

#include <memory>
#include <functional>

#include "Event.h"
#include "Timer.h"


#define GET_ROBOT() smachine::StateMachine::class_instance->get_robot()

#define GET_ENV(T) smachine::Environment::class_instance->get_env<T>()
#define STM_FINISH() smachine::StateMachine::class_instance->finish();

#define STM_STATE_CHANGE(state) smachine::StateMachine::class_instance->state_change(state);
#define STM_STATE_CHANGE2FIRST() smachine::StateMachine::class_instance->state_change_to_first();
#define STM_STATE_POP() smachine::StateMachine::class_instance->state_pop();

#define STM_TIMER_CREATE(tm_info) smachine::StateMachine::class_instance->create_timer(tm_info);
#define STM_TIMER_CANCEL(id) smachine::StateMachine::class_instance->cancel_timer(id);
#define STM_TIMER_RESTART(id) smachine::StateMachine::class_instance->reset_timer(id);


namespace smachine {
namespace state {

/*
 * Return true from OnEvent and OnTimer if received Event or Timer
 * was processed inside function
 */

class State {
public:
	State(const std::string& name) : m_name(name){

	}

	virtual ~State(){

	}

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

	bool init_timer(const int id, const time_t tv_sec, long tv_nsec, bool interval);

private:
	std::string m_name;
};

} /* namespace state */
} /* namespace smachine */

#endif /* PI_SMACHINE_STATE_H_ */
