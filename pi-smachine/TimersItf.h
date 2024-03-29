/*
 * TimersItf.h
 *
 *  Created on: Nov 27, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_TIMERSITF_H_
#define PI_SMACHINE_TIMERSITF_H_

#include <stdint.h>
#include <memory>

#include "Timer.h"

namespace smachine {

class TimersItf {
public:
	TimersItf() {}
	virtual ~TimersItf() {}

	virtual bool create_timer(const std::shared_ptr<Timer> timer) = 0;
	virtual bool cancel_timer(const int id) = 0;
	virtual bool reset_timer(const int id) = 0;
	//detect is timer active now
	virtual bool is_timer(const int id) = 0;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_TIMERSITF_H_ */
