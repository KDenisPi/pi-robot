/*
 * StateMachineItf.h
 *
 *  Created on: Nov 24, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_STATEMACHINEITF_H_
#define PI_SMACHINE_STATEMACHINEITF_H_

#include <string>
#include <memory>

#include "TimersItf.h"
#include "PiRobot.h"
#include "Environment.h"
#include "MqttDefines.h"
#include "Event.h"

namespace smachine {

class StateMachineItf {
public:
	StateMachineItf(){}
	virtual ~StateMachineItf(){}

	// Generate finish signal
	virtual void finish() = 0;
	// create event for state changing
	virtual void state_change(const std::string& new_state) = 0;
	virtual const std::string get_first_state() = 0;

	virtual void add_event(const std::shared_ptr<Event>& event) = 0;
	//
	virtual void state_pop() = 0;
	// create timer
	virtual bool timer_start(const int timer_id, const time_t interval, const bool interval_timer = false) = 0;
	// delete timer
	virtual bool timer_cancel(const int timer_id) = 0;
	//check if timer is running
	virtual bool timer_check(const int timer_id) = 0;

	virtual std::shared_ptr<pirobot::PiRobot> get_robot() = 0;
	virtual std::shared_ptr<Environment> get_env() = 0;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_STATEMACHINEITF_H_ */
