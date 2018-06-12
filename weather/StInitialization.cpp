/*
 * StInitialization.cpp
 * 
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#include "StInitialization.h"
#include "context.h"


namespace weather {

void StInitialization::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    //CHANGE_STATE("StMeasurement");
    CHANGE_STATE("StInitializeLcd");

}

bool StInitialization::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            get_itf()->finish();
            return true;
        }
        break;
    }

    return false;
}

bool StInitialization::OnEvent(const std::shared_ptr<smachine::Event> event){

    return false;
}

void StInitialization::OnSubstateExit(const std::string substate_name) {

    if(substate_name == "StInitializeSensors"){
        CHANGE_STATE("StMeasurement");
    }
}


}//namespace weather
