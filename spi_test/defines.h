/*
 * defines.h
 *
 *  Created on: Dec 8, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_DEFINES_H_
#define PROJECT1_DEFINES_H_

#include "logger.h"

enum ID {
	TIMER_SWITCH_TO_SECOND = 1000,
	TIMER_FINISH_ROBOT = 1001,
	TIMER_USER_1 = 1002
};

#define EVT_CYCLE_FINISHED   "CYCLE_FN"

#endif /* PROJECT1_DEFINES_H_ */
