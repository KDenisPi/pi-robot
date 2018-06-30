/*
 * StateFactory.h
 *
 *  Created on: Nov 24, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_STATEFACTORY_H_
#define PI_SMACHINE_STATEFACTORY_H_

#include <string>

#include "State.h"
#include "Environment.h"

namespace smachine {

class StateFactory {
public:
	StateFactory(const std::string& firstState) : _firstState(firstState) {

	}

	virtual ~StateFactory() {

	}

	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string state_name, smachine::StateMachineItf* itf) noexcept(false) = 0;

	/*
	 * Create Environment object
	 */
	virtual Environment* get_environment() = 0;

	/*
	* Return the first state of State Machine
	*/
	const std::string get_first_state() const {
		return _firstState;
	}

private:
	std::string _firstState;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_STATEFACTORY_H_ */
