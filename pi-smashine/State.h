/*
 * State.h
 *
 *  Created on: Nov 11, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_STATE_H_
#define PI_SMASHINE_STATE_H_

#include <memory>
#include "Event.h"
#include "Timer.h"

namespace smashine {

class State {
public:
	State() {}
	virtual ~State() {}

	virtual void OnEntry() = 0;
	virtual bool OnEvent(const std::shared_ptr<Event> event) = 0;
	virtual bool OnExit() = 0;
	virtual bool OnTimer(const std::shared_ptr<Timer> timer) = 0;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_STATE_H_ */
