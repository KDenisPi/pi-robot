/*
 * factory.h
 *
 *  Created on: December 9, 2021
 *      Author: Denis Kudia
 */

#ifndef TMAIN_FACTORY_H_
#define TMAIN_FACTORY_H_

#include "ststates.h"
#include "state_next.h"

namespace tmain {

class TMainStFactory: public smachine::StateFactory {
public:
	TMainStFactory(const std::string& firstState = "StInit") : smachine::StateFactory(firstState) {
		logger::log(logger::LLOG::DEBUG, "tmain", std::string(__func__) + " Set first state:" + firstState);
	}

	virtual ~TMainStFactory() {}

	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string& state_name) override {
        logger::log(logger::LLOG::DEBUG, "tmain", std::string(__func__) + " State:" + state_name);

        if(state_name.compare("StInitialization") == 0){
            return std::shared_ptr<smachine::state::State>(new tmain::StInit());
        }

        if(state_name.compare("StNext") == 0){
            return std::shared_ptr<smachine::state::State>(new tmain::StNext());
        }

        //return empty object
        return smachine::StateFactory::get_state(state_name);
    }
};

} //end namespace
#endif