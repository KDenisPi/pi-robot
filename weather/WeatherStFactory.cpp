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

namespace weather {

WeatherStFactory::WeatherStFactory() {
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

}

WeatherStFactory::~WeatherStFactory() {
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
}

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
	

	logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Generate exception no such State");
	throw std::runtime_error("No such state");
}


} /* namespace weather */
