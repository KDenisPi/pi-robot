/*
 * State.h
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
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
	State(StateMachineItf* itf, const std::string name);
	virtual ~State();

	virtual void OnEntry() = 0;
	virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) {return false;};
	virtual void OnExit() {}
	virtual bool OnTimer(const int id) {return false;};

	StateMachineItf* get_itf() const { return m_itf;}
	std::shared_ptr<pirobot::PiRobot> get_robot() const { return m_itf->get_robot();}

	inline const std::string get_name() { return m_name;}
	inline bool operator==(const State& state){
		return (m_name == state.m_name);
	}


private:
	StateMachineItf* m_itf;
	std::string m_name;
};

} /* namespace state */
} /* namespace smachine */

#endif /* PI_SMACHINE_STATE_H_ */
