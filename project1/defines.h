/*
 * defines.h
 *
 *  Created on: Dec 8, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_DEFINES_H_
#define PROJECT1_DEFINES_H_

#include "logger.h"

const char TAG[] = "proj1";

enum ID {
	TIMER_SWITCH_TO_SECOND = 1000,
	TIMER_FINISH_ROBOT = 1001,
	TIMER_LIGHT_METER_STOP = 1002,
	TIMER_SGP30_Get_VALUE = 1003,
};

#endif /* PROJECT1_DEFINES_H_ */
