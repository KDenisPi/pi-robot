/*
 * TimersItf.h
 *
 *  Created on: Nov 27, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_TIMERSITF_H_
#define PI_SMASHINE_TIMERSITF_H_

#include <stdint.h>

namespace smashine {

class TimersItf {
public:
	TimersItf() {}
	virtual ~TimersItf() {}

	virtual bool create_timer(const uint16_t id) = 0;
	virtual void cancel_timer(const uint16_t id) = 0;
	virtual void reset_timer(const uint16_t id) = 0;
};

} /* namespace smashine */

#endif /* PI_SMASHINE_TIMERSITF_H_ */
