/*
 * MyStateFactory.cpp
 *
 *  Created on: Nov 30, 2016
 *      Author: denis
 */

#include "MyStateFactory.h"
#include "StateEnvAnalize.h"

namespace project1 {

MyStateFactory::MyStateFactory() {
	// TODO Auto-generated constructor stub

}

MyStateFactory::~MyStateFactory() {
	// TODO Auto-generated destructor stub
}

const std::shared_ptr<smashine::state::State> MyStateFactory::get_state(const std::string state_name,
		const std::shared_ptr<smashine::StateMashineItf> itf)
{
	if(state_name.compare("StateEnvAnalize") == 0){
		return std::shared_ptr<smashine::state::State>(new project1::state::StateEnvAnalize(itf));
	}

	//TODO: Generate exception - State not found
	return nullptr;
}


} /* namespace project1 */
