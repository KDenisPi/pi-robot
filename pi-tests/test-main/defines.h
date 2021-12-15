/*
 * defines.h
 *
 *  Created on: Dec 10, 2021
 *      Author: Denis Kudia
 */

#ifndef TMAIN_DEFINES_H_
#define TMAIN_DEFINES_H_

#include "logger.h"

namespace tmain {

const char TAG[] = "tmain";

enum ID {
    TIMER_1      = 1000,
    TIMER_2      = 1001,
    TIMER_3      = 1002,
    TIMER_4      = 1003,
    TIMER_5      = 1004,
};

#define EVT_1 "EVENT_1"
#define EVT_2 "EVENT_2"
#define EVT_3 "EVENT_3"

#define INTERVAL_5   5 //sec
#define INTERVAL_10  10 //sec
#define INTERVAL_15  15 //sec
#define INTERVAL_20  20 //sec

}
#endif /* TMAIN_DEFINES_H_ */
