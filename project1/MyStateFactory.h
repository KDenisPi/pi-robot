/*
 * MyStateFactory.h
 *
 *  Created on: Nov 30, 2016
 *      Author: denis
 */

#ifndef PROJECT1_MYSTATEFACTORY_H_
#define PROJECT1_MYSTATEFACTORY_H_

#include "StateFactory.h"

namespace project1 {

class MyStateFactory: public smachine::StateFactory {
public:
	MyStateFactory();
	virtual ~MyStateFactory();

	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string state_name, smachine::StateMachineItf* itf) override;

};

} /* namespace project1 */

#endif /* PROJECT1_MYSTATEFACTORY_H_ */
