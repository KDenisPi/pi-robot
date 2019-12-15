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
		logger::log(logger::LLOG::DEBUG, "StFact", std::string(__func__) + " State:" + firstState);
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

	/*
	* Configuration file used for environment initilizing
	*/
	const std::string& get_configuration(){
		return _conf_file;
	}

	void set_configuration(const std::string& conf_file){
		_conf_file = conf_file;
	}

private:
	std::string _firstState;
	std::string _conf_file;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_STATEFACTORY_H_ */
