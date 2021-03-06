/*
 * MyStateFactory.cpp
 *
 *  Created on: Nov 30, 2016
 *      Author: Denis Kudia
 */

#include "logger.h"
#include "MyStateFactory.h"
#include "StateEnvAnalize.h"
//#include "StateUpdateState.h"

const char TAG[] = "myfact";

namespace spi_test {

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
		return std::shared_ptr<smachine::state::State>(new spi_test::state::StateEnvAnalize(itf));
	}
	/*
	else if(state_name.compare("StateUpdateState") == 0){
		return std::shared_ptr<smachine::state::State>(new project1::state::StateUpdateState(itf));
	}
	*/

	logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Generate exception no such State");
	throw std::runtime_error("No such state");
}


} /* namespace project1 */
