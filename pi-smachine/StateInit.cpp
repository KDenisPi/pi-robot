/*
 * StateInit.cpp
 *
 *  Created on: Nov 18, 2016
 *      Author: denis
 */

#include "StateInit.h"
#include "logger.h"

namespace smachine {
namespace state {

const char TAG[] = "sinit";

StateInit::StateInit(const std::shared_ptr<StateMachineItf> itf):
		state::State(itf, "StateInit")
{
	// TODO Auto-generated constructor stub

}

StateInit::~StateInit() {
	// TODO Auto-generated destructor stub
}

void StateInit::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEntry ");
	/*
	 * Need to add hardware initialization there.
	 * Also to check hardware state and configuration
	 */

	//temporal solution
	get_itf()->state_change("StateEnvAnalize");

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEntry finished");
}

bool StateInit::OnEvent(const std::shared_ptr<Event> event){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEvent ");
	return false;
}

bool StateInit::OnTimer(const int id){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ");
	return false;
}


} /* namespace state */
} /* namespace smachine */
