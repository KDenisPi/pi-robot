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

	StateFactory(){}

	/**
	 * @brief Destroy the State Factory object
	 *
	 */
	virtual ~StateFactory() {
	    //std::cout <<  "StateFactory::~StateFactory finished" << std::endl;
	}

	/**
	 * @brief Get the state object
	 *
	 * @param state_name
	 * @return const std::shared_ptr<smachine::state::State>
	 */
	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string& state_name = ""){
		logger::log(logger::LLOG::DEBUG, "StFact", std::string(__func__) + " State:" + state_name);
		return std::make_shared<smachine::state::State>(state_name.empty() ? _firstState : state_name);
	}

	/**
	 * @brief Return the first state of State Machine
	 *
	 * @return const std::string
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
