/*
 * StateFactory.h
 *
 *  Created on: Nov 24, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_STATEFACTORY_H_
#define PI_SMASHINE_STATEFACTORY_H_

#include <string>

#include "State.h"

namespace smashine {

class StateFactory {
public:
	StateFactory();
	virtual ~StateFactory();

	virtual const std::shared_ptr<state::State> get_state(const std::string state_name) = 0;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_STATEFACTORY_H_ */
