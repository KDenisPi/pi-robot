/*
 * WeatherStFactory.h
 *
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_STATEFACTORY_H_
#define WEATHER_STATEFACTORY_H_

#include "StateFactory.h"
#include "context.h"
#include "StInitialization.h"
#include "StMeasurement.h"
#include "StInitializeSensors.h"
#include "StInitializeLcd.h"
#include "StNoHardware.h"

namespace weather {

class WeatherStFactory: public smachine::StateFactory {
public:
	WeatherStFactory(const std::string& firstState = "StInitialization") : smachine::StateFactory(firstState) {}
	virtual ~WeatherStFactory() {}

	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string& name, const std::shared_ptr<smachine::StateMachineItf>& itf) override {
		if(name.compare("StInitialization") == 0){
			return std::make_shared<weather::StInitialization>(itf);
		}
		else if(name.compare("StMeasurement") == 0){
			return std::make_shared<weather::StMeasurement>(itf);
		}
		else if(name.compare("StInitializeSensors") == 0){
			return std::make_shared<weather::StInitializeSensors>(itf);
		}
		else if(name.compare("StInitializeLcd") == 0){
			return std::make_shared<weather::StInitializeLcd>(itf);
		}
		else if(name.compare("StNoHardware") == 0){
			return std::make_shared<weather::StNoHardware>(itf);
		}

		return smachine::StateFactory::get_state(name, itf);
	}

	/*
	 * Create Environment object
	 */
	virtual const std::shared_ptr<smachine::Environment> get_environment() override {
		return std::make_shared<weather::Context>();
	}
};

} /* namespace weather */

#endif /* WEATHER_STATEFACTORY_H_ */
