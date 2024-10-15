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

#include "Timer.h"
#include "PiRobot.h"
#include "Environment.h"
#include "MqttDefines.h"
#include "Event.h"

namespace smachine {

class StateMachineItf {
public:
	StateMachineItf(){}
	virtual ~StateMachineItf(){}

	/**
	 * @brief Generate finish signal
	 *
	 */
	virtual void finish() = 0;

	/**
	 * @brief create event for state changing
	 *
	 * @param new_state
	 */
	virtual void state_change(const std::string& new_state) = 0;

	/**
	 * @brief Get the first state object
	 *
	 * @return const std::string
	 */
	virtual const std::string get_first_state() = 0;

	/**
	 * @brief
	 *
	 * @param event
	 */
	virtual void add_event(const std::shared_ptr<Event>& event) = 0;

	/**
	 * @brief
	 *
	 */
	virtual void state_pop() = 0;

	virtual std::shared_ptr<pirobot::PiRobot> get_robot() = 0;
	virtual std::shared_ptr<Environment> get_env() = 0;

	/**
	 * @brief Create a timer object
	 *
	 * @param tm_info
	 * @return true
	 * @return false
	 */
	virtual bool create_timer(const struct timer::timer_info& tm_info) = 0;

	/**
	 * @brief Cancel timer (stop timer)
	 *
	 * @param id
	 * @return true
	 * @return false
	 */
	virtual bool cancel_timer(const int id) = 0;

	/**
	 * @brief Reset timer
	 *
	 * @param id
	 * @return true
	 * @return false
	 */
	virtual bool reset_timer(const int id) = 0;

	/**
	 * @brief Detect is timer active now
	 *
	 * @param id
	 * @return true
	 * @return false
	 */
	virtual bool is_timer(const int id) = 0;
};

} /* namespace smachine */

#endif /* PI_SMACHINE_STATEMACHINEITF_H_ */
