/*
 * Timer.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#include <signal.h>
#include <time.h>

#include "Timer.h"
#include "logger.h"

namespace smachine {
namespace timer {

const char TAG[] = "timer";

/**
 * @brief Destroy the Timer:: Timer object
 *
 */
Timer::~Timer() {
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(_id) + " IID: " + std::to_string((uintmax_t)tid));

    if(timer_delete(tid) < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not delete timer Error: " + std::to_string(errno));
    }
}

/**
 * @brief
 *
 * @param id
 * @param tv_sec
 * @param tv_nsec
 * @param interval
 * @return const TimerPtr
 */
const TimerPtr Timer::create(const struct timer_info& tm_info){
    struct sigevent evt;
    struct itimerspec itime;
	timer_t tid;

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(tm_info.id) +
        " Sec: " + std::to_string(tm_info.tv_sec) + " NSec: "+ std::to_string(tm_info.tv_nsec) + " Interval: " + std::to_string(tm_info.interval));

    clockid_t clockid = CLOCK_REALTIME;
    evt.sigev_notify = SIGEV_SIGNAL;
    evt.sigev_signo = TIMER_SIG;
    evt.sigev_value.sival_int =  tm_info.id;	//Using for timer recognizing
    evt.sigev_notify_function = NULL;
    evt.sigev_notify_attributes = NULL;

	auto result =  std::make_shared<smachine::timer::Timer>(tm_info);
    int res = timer_create(clockid, &evt, &result->tid);
    if(res < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not create timer Error: " + std::to_string(errno) +
            " CLID: " + std::to_string(clockid));
        result.reset();
    }
    else{
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer IID: " + std::to_string((uintmax_t)result->tid));
    }

	return result;
}

/**
 * @brief Start timer
 *
 * @return true
 * @return false
 */
const bool Timer::start(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(_id));

	struct itimerspec itimes = itime;

    const auto sinfo = info_to_string(itimes);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + sinfo);


    if(timer_settime(tid, 0, &itimes, NULL) < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not start timer Error: " + std::to_string(errno));
        return false;
    }

	return true;
}

/**
 * @brief Stop timer
 *
 * @return true
 * @return false
 */
const bool Timer::stop(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " timer ID: " + std::to_string(_id));

	struct itimerspec itimes = {0,0,0,0};
    if(timer_settime(tid, 0, &itimes, NULL) < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " could not stop timer Error: " + std::to_string(errno));
        return false;
    }

	return true;
}

}
} /* namespace smachine */
