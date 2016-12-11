/*
 * StateFactory.h
 *
 *  Created on: Nov 24, 2016
 *      Author: denis
 */

#ifndef PI_SMACHINE_STATEFACTORY_H_
#define PI_SMACHINE_STATEFACTORY_H_

#include <string>
#include <exception>

#include "State.h"

namespace smachine {

class StateFactory {
public:
	StateFactory();
	virtual ~StateFactory();

	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string state_name, const std::shared_ptr<smachine::StateMachineItf> itf) = 0;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_STATEFACTORY_H_ */
