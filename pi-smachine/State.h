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

#define TIMER_CREATE(id, interval)  get_itf()->timer_start(id, interval);
#define TIMER_CANCEL(id)  get_itf()->timer_cancel(id);
#define CHANGE_STATE(state) get_itf()->state_change(state);
#define POP_STATE() get_itf()->state_pop();
#define EVENT(evt) get_itf()->add_event(evt);
/*
 * Return true from OnEvent and OnTimer if received Event or Timer
 * was processed inside function
 */

class State {
public:
	State(StateMachineItf* itf, const std::string name);
	virtual ~State();

	virtual void OnEntry() {}
	virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) {return false;};
	virtual void OnExit() {}
	virtual bool OnTimer(const int id) {return false;};
	//called when child state call POP
	virtual void OnSubstateExit(const std::string substate_name) {}

	StateMachineItf* get_itf() const { return m_itf;}
	std::shared_ptr<pirobot::PiRobot> get_robot() const { return m_itf->get_robot();}

	inline const std::string get_name() { return m_name;}
	inline bool operator==(const State& state){
		return (m_name == state.m_name);
	}

	//Get Item by name
	template<class T> std::shared_ptr<T> get_item(const std::string name){
		return std::static_pointer_cast<T>(get_itf()->get_robot()->get_item(name));
	}

	/*
	* Get Environment
	*/
	template<class T> std::shared_ptr<T> get_env(){
		return std::static_pointer_cast<T>(get_itf()->get_env());
	}

private:
	StateMachineItf* m_itf;
	std::string m_name;
};

} /* namespace state */
} /* namespace smachine */

#endif /* PI_SMACHINE_STATE_H_ */
