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
#include <sys/time.h>
#include <signal.h>
#include <iostream>

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

    /*
    *
    */
   static int add_signal(int signal){
        sigset_t new_set, org_set;
        int res;

        sigemptyset (&new_set);
        sigemptyset (&org_set);

        if(signal>0){
            res = sigaddset (&new_set, signal);
            res = sigprocmask(SIG_BLOCK, &new_set, &org_set);
        }

        return res;
   }

   static void print_sigset(const sigset_t* sigset){
       int count = 0;
       for(int sig = 1; sig < NSIG; sig++){
            if( sigismember(sigset, sig) > 0){
                std::cout <<  "Sig blocked: " << sig << std::endl;
                count++;
            }
       }
        if(count==0)
            std::cout <<  "All signals available: " << std::endl;
   }
};

}//namespace timers
} //namespace piutils
#endif

