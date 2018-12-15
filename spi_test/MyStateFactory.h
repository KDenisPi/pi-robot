/*
 * MyStateFactory.h
 *
 *  Created on: Nov 30, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_MYSTATEFACTORY_H_
#define PROJECT1_MYSTATEFACTORY_H_

#include "Environment.h"
#include "StateFactory.h"
#include "MyEnv.h"

namespace spi_test {


class MyStateFactory: public smachine::StateFactory {
public:
	MyStateFactory(const std::string& firstState = "StateEnvAnalize") : smachine::StateFactory(firstState) {}
	virtual ~MyStateFactory() {}

	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string state_name, smachine::StateMachineItf* itf) noexcept(false) override;

	/*
	 * Create Environment object
	 */
	virtual smachine::Environment* get_environment() override;
};

} /* namespace project1 */

#endif /* PROJECT1_MYSTATEFACTORY_H_ */
