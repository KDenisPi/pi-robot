/*
 * TimersItf.h
 *
 *  Created on: Nov 27, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_TIMERSITF_H_
#define PI_SMASHINE_TIMERSITF_H_

#include <stdint.h>
#include <memory>

#include "Timer.h"

namespace smashine {

class TimersItf {
public:
	TimersItf() {}
	virtual ~TimersItf() {}

	virtual bool create_timer(const std::shared_ptr<Timer> timer) = 0;
	virtual void cancel_timer(const int id) = 0;
	virtual void reset_timer(const int id) = 0;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_TIMERSITF_H_ */
