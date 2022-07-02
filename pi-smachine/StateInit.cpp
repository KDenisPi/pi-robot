/*
 * StateInit.cpp
 *
 *  Created on: Nov 18, 2016
 *      Author: Denis Kudia
 */

#include "StateInit.h"
#include "logger.h"

namespace smachine {
namespace state {

const char TAG[] = "sinit";

void StateInit::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEntry ");

	/*
	 * Need to add hardware initialization there.
	 * Also to check hardware state and configuration
	 */
	if( !get_robot()->configure("")){
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Hardware configuration failed. Stop.");
		finish(); //Generate FINISH Event
	}

	/*
	 * Start Hardware components
	 */
	if( !get_robot()->start()){
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Hardware configuration failed. Stop.");
		//Generate FINISH Event
		finish();
	}

	//temporal solution
	state_change(get_first_state());

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
