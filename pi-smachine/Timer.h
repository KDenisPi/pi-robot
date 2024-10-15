/*
 * Timer.h
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_TIMER_H_
#define PI_SMACHINE_TIMER_H_

#include <time.h>
#include <memory>

namespace smachine {
namespace timer {

#define TIMER_SIG SIGRTMIN+1

class Timer;
using TimerPtr = std::shared_ptr<smachine::timer::Timer>;

struct timer_info {
    int id;
    time_t tv_sec;
    long tv_nsec;
    bool interval;
};

/**
 * @brief
 *
 */
class Timer {
public:
	Timer(const struct timer_info& tm_info) :
		_id(tm_info.id), tid(nullptr), _interval(tm_info.interval) {

		if(_interval){
			itime.it_interval.tv_sec = tm_info.tv_sec;
			itime.it_interval.tv_nsec = tm_info.tv_nsec;

			itime.it_value.tv_sec = tm_info.tv_sec;
			itime.it_value.tv_nsec = tm_info.tv_nsec;
		}
		else{
			itime.it_interval.tv_sec = 0;
			itime.it_interval.tv_nsec = 0;

			itime.it_value.tv_sec = tm_info.tv_sec;
			itime.it_value.tv_nsec = tm_info.tv_nsec;
		}
	}

	static const TimerPtr create(const struct timer_info& tm_info);

	Timer() = delete;
	virtual ~Timer();

	const bool start();
	const bool stop();

	const int get_id() const {return _id;}
	const timer_t get_tid() const {return tid;}
	const struct itimerspec& get_time() const {return itime;}
	bool is_interval() const {return _interval;}

	const std::string get_timeinfo() const{
		struct itimerspec curr_info;
		timer_gettime(tid, &curr_info);
		return info_to_string(curr_info);
	}

	const std::string info_to_string(const itimerspec& info) const {
		std::string res = std::string(" it_interval: ") + std::to_string(info.it_interval.tv_sec) + " sec. " + std::to_string(info.it_interval.tv_nsec) + " nsec. ";
		res = res + std::string("it_value: ") + std::to_string(info.it_value.tv_sec) + " sec. " + std::to_string(info.it_value.tv_nsec) + " nsec. ";

		return res;
	}

protected:
	timer_t tid;

private:
	bool _interval = false;
	int _id;
	struct itimerspec itime;
};

} /* namespace timer */
} /* namespace smachine */

#endif /* PI_SMACHINE_TIMER_H_ */
