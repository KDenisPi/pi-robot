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
#include "StateMashineItf.h"

namespace smashine {
namespace state {

/*
 * Return true from OnEvent and OnTimer if received Event or Timer
 * was processed inside function
 */

class State {
public:
	State(const std::shared_ptr<StateMashineItf> itf, const std::shared_ptr<pirobot::PiRobot> robot);
	virtual ~State();

	virtual void OnEntry() = 0;
	virtual bool OnEvent(const std::shared_ptr<Event> event) {return false;};
	virtual void OnExit() {}
	virtual bool OnTimer(const int id) {return false;};

	std::shared_ptr<StateMashineItf> get_itf() const { return m_itf;}
	std::shared_ptr<pirobot::PiRobot> get_robot() const { return m_robot;}
private:
	std::shared_ptr<pirobot::PiRobot> m_robot;
	std::shared_ptr<StateMashineItf> m_itf;
};

} /* namespace state */
} /* namespace smashine */

#endif /* PI_SMASHINE_STATE_H_ */
