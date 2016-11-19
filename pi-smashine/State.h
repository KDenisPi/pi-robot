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
#include "PiRobot.h"

namespace smashine {

/*
 * Return true from OnEvent and OnTimer if received Event or Timer
 * was processed inside function
 */

class State {
public:
	State(const std::shared_ptr<pirobot::PiRobot> robot);
	virtual ~State();

	virtual void OnEntry() = 0;
	virtual bool OnEvent(const std::shared_ptr<Event> event) {return false;};
	virtual void OnExit() {}
	virtual bool OnTimer(const std::shared_ptr<Timer> timer) {return false;};

private:
	std::shared_ptr<pirobot::PiRobot> m_robot;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_STATE_H_ */
