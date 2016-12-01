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

class MyStateFactory: public smashine::StateFactory {
public:
	MyStateFactory();
	virtual ~MyStateFactory();

	virtual const std::shared_ptr<smashine::state::State> get_state(const std::string state_name, const std::shared_ptr<smashine::StateMashineItf> itf) override;

};

} /* namespace project1 */

#endif /* PROJECT1_MYSTATEFACTORY_H_ */
