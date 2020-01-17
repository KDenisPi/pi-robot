/*
 * MyStateFactory.cpp
 *
 *  Created on: Nov 30, 2016
 *      Author: Denis Kudia
 */

#include <exception>
#include <stdexcept>

#include "logger.h"
#include "MyStateFactory.h"
#include "StateEnvAnalize.h"
#include "StateUpdateState.h"
#include "StateNoHardware.h"
#include "StateWeather.h"


const char TAG[] = "myfact";

namespace project1 {

MyStateFactory::MyStateFactory()
	: smachine::StateFactory("StateWeather") {
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

}

/*
 * Create Environment object
 */
smachine::Environment* MyStateFactory::get_environment(){
	return new MyEnv();
}

/*
 *
 */
const std::shared_ptr<smachine::state::State> MyStateFactory::get_state(const std::string state_name,
		smachine::StateMachineItf* itf)
{
	if(state_name.compare("StateEnvAnalize") == 0){
		return std::shared_ptr<smachine::state::State>(new project1::state::StateEnvAnalize(itf));
	}
	else if(state_name.compare("StateUpdateState") == 0){
		return std::shared_ptr<smachine::state::State>(new project1::state::StateUpdateState(itf));
	}
	else if(state_name.compare("StateNoHardware") == 0){
		return std::shared_ptr<smachine::state::State>(new project1::state::StateNoHardware(itf));
	}
	else if(state_name.compare("StateWeather") == 0){
		return std::shared_ptr<smachine::state::State>(new project1::state::StateWeather(itf));
	}

	//return empty object
	return this->StateFactory::get_state(state_name, itf);
}


} /* namespace project1 */
