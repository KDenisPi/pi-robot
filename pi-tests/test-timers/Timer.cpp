/*
 * Timer.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: Denis Kudia
 */

#include "Timer.h"

namespace smachine {
namespace timer {

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

/**
 * @brief
 *
 * @return const std::shared_ptr<smachine::Timer>
 */
const TimerPtr Timer::create(const int id){
	return std::shared_ptr<smachine::timer::Timer>(nullptr);
}

}
} /* namespace smachine */
