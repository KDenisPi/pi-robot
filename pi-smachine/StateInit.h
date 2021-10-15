/*
 * StateInit.h
 *
 *  Created on: Nov 18, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_STATEINIT_H_
#define PI_SMACHINE_STATEINIT_H_

#include "State.h"

namespace smachine {
namespace state {

class StateInit: public State {
public:
	StateInit(const std::shared_ptr<smachine::StateMachineItf>& itf);
	virtual ~StateInit();

	virtual void OnEntry() override;
	virtual bool OnEvent(const std::shared_ptr<Event> event) override;
	virtual bool OnTimer(const int id) override;

};

} /* namespace state */
} /* namespace smachine */

#endif /* PI_SMACHINE_STATEINIT_H_ */
