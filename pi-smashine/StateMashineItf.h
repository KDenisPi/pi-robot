/*
 * StateMashineItf.h
 *
 *  Created on: Nov 24, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_STATEMASHINEITF_H_
#define PI_SMASHINE_STATEMASHINEITF_H_

#include <string>
#include <memory>

#include "TimersItf.h"
#include "PiRobot.h"

namespace smashine {

class StateMashineItf {
public:
	StateMashineItf();
	virtual ~StateMashineItf();

	virtual void state_change(const std::string new_state) = 0;
	virtual void state_pop() = 0;
	virtual void timer_start(const int timer_id, const time_t interval, const bool interval_timer = false) = 0;
	virtual void timer_cancel(const int timer_id) = 0;
	virtual const std::shared_ptr<TimersItf> get_timersitf() = 0;
	virtual std::shared_ptr<pirobot::PiRobot> get_robot() = 0;

};

} /* namespace smashine */

#endif /* PI_SMASHINE_STATEMASHINEITF_H_ */
