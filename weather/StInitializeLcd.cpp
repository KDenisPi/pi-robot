/*
 * StInitializeLcd.cpp
 * 
 *  Created on: June 01, 2018
 *      Author: Denis Kudia
 */

#include "StInitializeLcd.h"
#include "context.h"


namespace weather {

void StInitializeLcd::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    //CHANGE_STATE("StMeasurement");
   TIMER_CREATE(TIMER_WARM_INTERVAL, 30) //wait for 15 seconds before real use
}

bool StInitializeLcd::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            TIMER_CANCEL(TIMER_WARM_INTERVAL);
            get_itf()->finish();
            return true;
        }
        //switch to main state
        case TIMER_WARM_INTERVAL:
        {
            get_itf()->finish();
            return true;
        }
    }

    return false;
}

bool StInitializeLcd::OnEvent(const std::shared_ptr<smachine::Event> event){

    return false;
}

}//namespace weather
