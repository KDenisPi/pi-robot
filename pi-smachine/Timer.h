/*
 * Timer.h
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_TIMER_H_
#define PI_SMACHINE_TIMER_H_

#include <time.h>

namespace smachine {

class Timer {
public:
	Timer(const int id, const time_t tv_sec, const long tv_nsec = 0, const bool interval = false);
	virtual ~Timer();

	const int get_id() const { return m_id;}
	const timer_t get_tid() const { return m_tid;}
	const struct timespec& get_time() const { return m_itime;}
	bool is_interval() const {return m_interval;}

	timer_t m_tid;
private:
	bool m_interval;
	int m_id;
	struct timespec m_itime;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_TIMER_H_ */
