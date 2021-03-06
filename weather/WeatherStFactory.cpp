/*
 * WeatherStFactory.cpp
 *
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#include <exception>
#include <stdexcept>

#include "logger.h"

#include "defines.h"
#include "WeatherStFactory.h"
#include "StInitialization.h"
#include "StMeasurement.h"
#include "StInitializeSensors.h"
#include "StInitializeLcd.h"
#include "StNoHardware.h"

namespace weather {

/*
 *
 */
const std::shared_ptr<smachine::state::State> WeatherStFactory::get_state(const std::string state_name,	smachine::StateMachineItf* itf)
{
	if(state_name.compare("StInitialization") == 0){
		return std::shared_ptr<smachine::state::State>(new weather::StInitialization(itf));
	}
	else if(state_name.compare("StMeasurement") == 0){
		return std::shared_ptr<smachine::state::State>(new weather::StMeasurement(itf));
	}
	else if(state_name.compare("StInitializeSensors") == 0){
		return std::shared_ptr<smachine::state::State>(new weather::StInitializeSensors(itf));
	}
	else if(state_name.compare("StInitializeLcd") == 0){
		return std::shared_ptr<smachine::state::State>(new weather::StInitializeLcd(itf));
	}
	else if(state_name.compare("StNoHardware") == 0){
		return std::shared_ptr<smachine::state::State>(new weather::StNoHardware(itf));
	}

	logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Generate exception no such State");
	throw std::runtime_error("No such state");
}


} /* namespace weather */
