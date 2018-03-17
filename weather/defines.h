/*
 * defines.h
 *
 *  Created on: March 16, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_DEFINES_H_
#define WEATHER_DEFINES_H_

#include "logger.h"

namespace weather {

const char TAG[] = "wthr";

enum ID {
    TIMER_SWITCH_TO_SECOND = 1000,
    TIMER_FINISH_ROBOT = 1001,
    TIMER_LIGHT_METER_STOP = 1002,
    TIMER_GET_VALUE = 1003,
};

}
#endif /* WEATHER_DEFINES_H_ */
