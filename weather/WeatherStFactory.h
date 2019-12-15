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

namespace weather {

class WeatherStFactory: public smachine::StateFactory {
public:
	WeatherStFactory(const std::string& firstState = "StInitialization") : smachine::StateFactory(firstState) {
		logger::log(logger::LLOG::DEBUG, "WtFact", std::string(__func__) + " Set first state:" + firstState);
	}

	virtual ~WeatherStFactory() {}

	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string state_name, smachine::StateMachineItf* itf) noexcept(false) override;

	/*
	 * Create Environment object
	 */
	virtual smachine::Environment* get_environment() override {
		return new Context();
	}
};

} /* namespace weather */

#endif /* WEATHER_STATEFACTORY_H_ */
