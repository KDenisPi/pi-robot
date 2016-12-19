/*
 * StateEnvAnalize.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: denis
 */

#include <vector>

#include "defines.h"
#include "StateEnvAnalize.h"
#include "MyEnv.h"

#include "led.h"

namespace project1 {
namespace state {

StateEnvAnalize::StateEnvAnalize(smachine::StateMachineItf* itf) :
		smachine::state::State(itf, "StateEnvAnalize")
{
	// TODO Auto-generated constructor stub

}

StateEnvAnalize::~StateEnvAnalize() {
	// TODO Auto-generated destructor stub
}

void StateEnvAnalize::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEntry started");

	/*
	 * Light On first LED
	 */
	try{
		auto env = dynamic_cast<MyEnv*>(get_itf()->get_env().get());
		//std::vector<std::string> items = get_robot()->get_items();
		std::string name = "LED_" + std::to_string(env->items[0]);
		logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " First Item: " + name);

		auto led = dynamic_cast<pirobot::item::Led*>(get_robot()->get_item(name).get());
		led->On();

		env->led_processed++;
	}
	catch(std::runtime_error& exc){

	}

	get_itf()->timer_start(TIMER_SWITCH_TO_SECOND, 4);

}

bool StateEnvAnalize::OnTimer(const int id){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer started ID: " + std::to_string(id));

	auto env = dynamic_cast<MyEnv*>(get_itf()->get_env().get());

	switch(id){
	case TIMER_SWITCH_TO_SECOND:
	{
		/*
		 * Light On second LED
		 */

		logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Processed: " + std::to_string(env->led_processed));

		//std::vector<std::string> items = get_robot()->get_items();
		if(env->led_processed < env->led_max){
			try{
				std::string led_name_prev = "LED_" + std::to_string(env->items[env->led_processed-1]);
				std::string led_name_next = "LED_" + std::to_string(env->items[env->led_processed]);

				logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Curr: " + led_name_prev + " Next: " + led_name_next);

				auto led_prev = dynamic_cast<pirobot::item::Led*>(get_robot()->get_item(led_name_prev).get());
				led_prev->Off();

				auto led_next = dynamic_cast<pirobot::item::Led*>(get_robot()->get_item(led_name_next).get());
				led_next->On();

				env->led_processed++;
			}
			catch(std::runtime_error& exc){

			}
			get_itf()->timer_start(TIMER_SWITCH_TO_SECOND, 4);
		}
		else {
			try{
				std::string led_name_prev = "LED_" + std::to_string(env->items[env->led_processed-1]);

				logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Curr: " + led_name_prev);

				auto led_prev = dynamic_cast<pirobot::item::Led*>(get_robot()->get_item(led_name_prev).get());
				led_prev->Off();
			}
			catch(std::runtime_error& exc){

			}
			/*
			 * Switch to another state
			 */
			get_itf()->state_change("StateUpdateState");

		}
		return true;
	}
	case TIMER_FINISH_ROBOT:
		get_itf()->finish();
		return true;
	}

	return false;
}


} /* namespace state */
} /* namespace project1 */
