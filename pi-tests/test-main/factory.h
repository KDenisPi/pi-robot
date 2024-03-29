/*
 * factory.h
 *
 *  Created on: December 9, 2021
 *      Author: Denis Kudia
 */

#ifndef TMAIN_FACTORY_H_
#define TMAIN_FACTORY_H_

#include "Environment.h"

namespace tmain {

class Context : public smachine::Environment {
public:
    Context(){}
    virtual ~Context() noexcept {}
};

class TMainStFactory: public smachine::StateFactory {
public:
	TMainStFactory(const std::string& firstState = "StInit") : smachine::StateFactory(firstState) {
		logger::log(logger::LLOG::DEBUG, "tmain", std::string(__func__) + " Set first state:" + firstState);
	}

	virtual ~TMainStFactory() {}

	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string& state_name, smachine::StateMachineItf* itf) override {
        logger::log(logger::LLOG::DEBUG, "tmain", std::string(__func__) + " State:" + state_name);

        if(state_name.compare("StInitialization") == 0){
	        ////std::cout  "Create StInit" << std::endl;

            return std::shared_ptr<smachine::state::State>(new tmain::StInit(itf));
        }

        //return empty object
        return smachine::StateFactory::get_state(state_name, itf);
    }

	/*
	 * Create Environment object
	 */
	virtual std::shared_ptr<smachine::Environment> get_environment() override {
		return std::make_shared<Context>();
	}
};

} //end namespace
#endif