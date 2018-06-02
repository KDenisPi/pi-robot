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
    TIMER_FINISH_ROBOT      = 1000,
    TIMER_WARM_INTERVAL     = 1001,
    TIMER_UPDATE_INTERVAL   = 1002,
    TIMER_LCD_INTERVAL      = 1003,
    TIMER_USER_INTERVAL     = 1004
};

}
#endif /* WEATHER_DEFINES_H_ */
