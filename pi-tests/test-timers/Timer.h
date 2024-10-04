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

/**
 * @brief
 *
 */
class Timer {
public:
	Timer(const int id, const time_t tv_sec = 1, const long tv_nsec = 0, const bool interval = false) :
		_id(id), tid(nullptr), _interval(interval) {

		if(interval){
			itime.it_interval.tv_sec = tv_sec;
			itime.it_interval.tv_nsec = tv_nsec;

			itime.it_value.tv_sec = tv_sec;
			itime.it_value.tv_nsec = tv_nsec;
		}
		else{
			itime.it_interval.tv_sec = 0;
			itime.it_interval.tv_nsec = 0;

			itime.it_value.tv_sec = tv_sec;
			itime.it_value.tv_nsec = tv_nsec;
		}
	}

	static const TimerPtr create(const int id, const time_t tv_sec = 1, const long tv_nsec = 0, const bool interval = false);

	Timer() = delete;
	virtual ~Timer();

	const bool start();
	const bool stop();

	const int get_id() const {return _id;}
	const timer_t get_tid() const {return tid;}
	const struct itimerspec& get_time() const {return itime;}
	bool is_interval() const {return _interval;}

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
