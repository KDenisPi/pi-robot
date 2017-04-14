/*
 * StateUpdateState.h
 *
 *  Created on: Dec 8, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_STATEUPDATESTATE_H_
#define PROJECT1_STATEUPDATESTATE_H_

#include <State.h>

namespace project1 {
namespace state {

class StateUpdateState: public smachine::state::State {
public:
	StateUpdateState(smachine::StateMachineItf* itf);
	virtual ~StateUpdateState();

	virtual void OnEntry() override;
	virtual bool OnTimer(const int id) override;

};

} /* namespace state */
} /* namespace project1 */

#endif /* PROJECT1_STATEUPDATESTATE_H_ */
