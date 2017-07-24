/*
 * StateEnvAnalize.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: Denis Kudia
 */

#include <vector>

#include "defines.h"
#include "StateEnvAnalize.h"
#include "MyEnv.h"

#include "led.h"
#include "blinking.h"
//#include "TiltSwitch.h"

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
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize started");

	auto red = dynamic_cast<pirobot::item::Blinking<pirobot::item::Led>*>(get_itf()->get_robot()->get_item("BLNK_Red").get());
	auto blue = dynamic_cast<pirobot::item::Blinking<pirobot::item::Led>*>(get_itf()->get_robot()->get_item("BLNK_Blue").get());
	auto yellow = dynamic_cast<pirobot::item::Blinking<pirobot::item::Led>*>(get_itf()->get_robot()->get_item("BLNK_Yellow").get());

	red->On();
	blue->On();
	yellow->On();

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize finished");
}

bool StateEnvAnalize::OnTimer(const int id){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer started ID: " + std::to_string(id));

/*
	switch(id){
	case TIMER_FINISH_ROBOT:
		get_itf()->finish();
		return true;
	}
*/
	return false;
}

bool StateEnvAnalize::OnEvent(const std::shared_ptr<smachine::Event> event){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ==== OnEvent Type: " + std::to_string(event->type()) +
			" Name:" + event->name());

	if(event->name().compare("BTN_Stop") == 0){
		if(event->type() == smachine::EVENT_TYPE::EVT_BTN_DOWN){
			get_itf()->finish();
			return true;
		}
	}

	return false;
}

} /* namespace state */
} /* namespace project1 */
