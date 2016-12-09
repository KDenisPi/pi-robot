/*
 * StateInit.cpp
 *
 *  Created on: Nov 18, 2016
 *      Author: denis
 */

#include "StateInit.h"

namespace smashine {
namespace state {

StateInit::StateInit(const std::shared_ptr<StateMashineItf> itf):
		state::State(itf)
{
	// TODO Auto-generated constructor stub

}

StateInit::~StateInit() {
	// TODO Auto-generated destructor stub
}

void StateInit::OnEntry(){
	/*
	 * Need to add hardware initialization there.
	 * Also to check hardware state and configuration
	 */

	//temporal solution
	get_itf()->state_change("StateEnvAnalize");
}

bool StateInit::OnEvent(const std::shared_ptr<Event> event){
	return false;
}

bool StateInit::OnTimer(const int id){
	return false;
}


} /* namespace state */
} /* namespace smashine */
