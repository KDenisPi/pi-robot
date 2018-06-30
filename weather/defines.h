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
    TIMER_USER_INTERVAL     = 1004,
    TIMER_IP_CHECK_INTERVAL = 1005,
    TIMER_SHOW_DATA_INTERVAL= 1006,
    TIMER_MEASURE_LIGHT_INTERVAL= 1007
    TIMER_WRITE_DATA_INTERVAL= 1008
};

#define EVT_SHOW_IP "SHOW_IP"
#define EVT_LCD_ON  "LCD_ON"
#define EVT_LCD_OFF "LCD_OFF"
#define EVT_HIGH_LEVEL_ON   "HIGH_ON"
#define EVT_HIGH_LEVEL_OFF  "HIGH_OFF"

#define MEASURE_INTERVAL        10 //sec
#define MEASURE_SHOW_INTERVAL   40 //sec
#define MEASURE_WRITE_INTERVAL  30 //sec
#define MEASURE_LIGHT_INTERVAL  1  //sec
#define IP_CHECK_INTERVAL       75 //sec

}
#endif /* WEATHER_DEFINES_H_ */
