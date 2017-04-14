/*
 * Timer.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#include "Timer.h"

namespace smachine {

Timer::	Timer(const int id, const time_t tv_sec, const long tv_nsec, const bool interval) :
		m_id(id), m_tid(0), m_itime({tv_sec, tv_nsec}), m_interval(interval)
{
	// TODO Auto-generated constructor stub

}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

} /* namespace smachine */
