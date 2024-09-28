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

class Timer;
using TimerPtr = std::shared_ptr<smachine::timer::Timer>;

class Timer {
protected:
	Timer(const int id, const time_t tv_sec = 1, const long tv_nsec = 0, const bool interval = false) :
		_id(id), tid(0), _itime({tv_sec, tv_nsec}), _interval(interval) {}

public:
	static const TimerPtr create(const int id);

	Timer() = delete;
	virtual ~Timer();

	const int get_id() const {return _id;}
	const timer_t get_tid() const {return tid;}
	const struct timespec& get_time() const {return _itime;}
	bool is_interval() const {return _interval;}
	void interval(const bool itv) {
		_interval = itv;
	}

	timer_t tid;
private:
	bool _interval = false;
	int _id;
	struct timespec _itime;
};

} /* namespace timer */
} /* namespace smachine */

#endif /* PI_SMACHINE_TIMER_H_ */
