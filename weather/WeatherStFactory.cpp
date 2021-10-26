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
const std::shared_ptr<smachine::state::State> WeatherStFactory::get_state(const std::string& state_name, const std::shared_ptr<smachine::StateMachineItf>& itf)
{
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " State:" + state_name);
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

	//return empty object
	return smachine::StateFactory::get_state(state_name, itf);
}


} /* namespace weather */
