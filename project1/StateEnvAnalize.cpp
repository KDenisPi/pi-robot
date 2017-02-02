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
#include "TiltSwitch.h"

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

	auto led = dynamic_cast<pirobot::item::Led*>(get_itf()->get_robot()->get_item("LED_1").get());
	auto tilt = dynamic_cast<pirobot::item::TiltSwitch*>(get_itf()->get_robot()->get_item("TILT_1").get());
	if(tilt->state() == pirobot::item::BUTTON_STATE::BTN_NOT_PUSHED)
		led->Off();
	else
		led->On();

}

bool StateEnvAnalize::OnTimer(const int id){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer started ID: " + std::to_string(id));

	switch(id){
	case TIMER_FINISH_ROBOT:
		get_itf()->finish();
		return true;
	}

	return false;
}

bool StateEnvAnalize::OnEvent(const std::shared_ptr<smachine::Event> event){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEvent Type: " + std::to_string(event->type()) +
			" Name:" + event->name());

	if(event->name().compare("BTN_1") == 0){
		if(event->type() == smachine::EVENT_TYPE::EVT_BTN_DOWN){
			get_itf()->finish();
		}
	}
	else if(event->name().compare("TILT_1") == 0){
		auto led = dynamic_cast<pirobot::item::Led*>(get_itf()->get_robot()->get_item("LED_1").get());

		if(event->type() == smachine::EVENT_TYPE::EVT_BTN_DOWN){
			led->On();
		}
		else if(event->type() == smachine::EVENT_TYPE::EVT_BTN_UP){
			led->Off();
		}
	}


	return false;
}

} /* namespace state */
} /* namespace project1 */
