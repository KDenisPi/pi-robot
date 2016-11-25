/*
 * StateMashineItf.h
 *
 *  Created on: Nov 24, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_STATEMASHINEITF_H_
#define PI_SMASHINE_STATEMASHINEITF_H_

#include <string>

namespace smashine {

class StateMashineItf {
public:
	StateMashineItf();
	virtual ~StateMashineItf();

	virtual void state_change(const std::string new_state) = 0;
	virtual void state_pop() = 0;
	virtual void timer_start(const int timer_id, const int interval) = 0;
	virtual void timer_stop(const int timer_id) = 0;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_STATEMASHINEITF_H_ */
