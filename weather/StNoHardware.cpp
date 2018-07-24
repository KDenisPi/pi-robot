/*
 * StNoHardware.cpp
 *
 *  Created on: June 29, 2018
 *      Author: Denis Kudia
 */

#include "StNoHardware.h"
#include "context.h"

namespace weather {

void StNoHardware::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    //TIMER_CREATE(TIMER_FINISH_ROBOT, 55) //wait for 15 seconds before finish
}

bool StNoHardware::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            get_itf()->finish();
            return true;
        }
    }

    return false;
}

bool StNoHardware::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + "  Event: " + event->to_string());
    return false;
}

}//namespace weather
