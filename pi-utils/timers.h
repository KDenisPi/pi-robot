/*
 * timers.h
 *
 *  Delay functions and other related to timers
 *
 *  Created on: Jun 19, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_UTILS_TIMERS_H_
#define PI_UTILS_TIMERS_H_

#include <time.h>

namespace piutils {
namespace timers {

using timesp = struct timespec;

class Timers {

public:
    /*
    * Interval in seconds
    */
    static int delay_sec(const int interval){
        timesp tm = {0,0};
        tm.tv_sec = interval;
        return nanosleep(&tm, &tm);
    }

    /*
    * Interval in milliseconds
    */
    static int delay(const int interval){
        timesp tm = {0,0};
        tm.tv_sec = interval/1000;
        tm.tv_nsec = (interval%1000)*1000000;
        return nanosleep(&tm, &tm);
    }

    /*
    * Interval in microseconds
    */
    static int delay_micro(const int interval){
        timesp tm = {0,0};
        tm.tv_nsec = interval*1000;
        return nanosleep(&tm, &tm);
    }

    /*
    * Interval in nano seconds
    */
    static int delay_nano(const int interval){
        timesp tm = {0,0};
        tm.tv_nsec = interval;
        return nanosleep(&tm, &tm);
    }

    static long milliseconds(){
        timesp tm;
        clock_gettime(CLOCK_MONOTONIC, &tm);
        return (tm.tv_sec*1000+tm.tv_nsec/1000000);
    }

};

}//namespace timers
} //namespace piutils
#endif

