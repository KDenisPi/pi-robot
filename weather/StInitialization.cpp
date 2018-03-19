/*
 * StInitialization.cpp
 * 
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#include "StInitialization.h"
#include "context.h"

#include "Si7021.h"
#include "sgp30.h"

namespace weather {

void StInitialization::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto context = get_env<weather::Context>();
    //make measurement using Si7021 and then use this values for SGP30
    auto si7021 = get_item<pirobot::item::Si7021>("SI7021");
    si7021->get_results(context->si7021_humidity, context->si7021_temperature, context->si7021_abs_humidity);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Result Humidity: " + std::to_string(context->si7021_humidity) + " Temperature: " + std::to_string(context->si7021_temperature) + "C");
    
    //use absolute humidity for SGP30
    auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
    sgp30->set_humidity(context->si7021_abs_humidity);
    sgp30->set_baseline(context->spg30_base_co2, context->spg30_base_tvoc);
    sgp30->set_initialized(true);

   TIMER_CREATE(TIMER_WARM_INTERVAL, 15) //wait for 15 seconds before real use
}

bool StInitialization::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            get_itf()->finish();
            return true;
        }
        //switch to main state
        case TIMER_WARM_INTERVAL:
        {
            CHANGE_STATE("StMeasurement");
            return true;
        }
    }

    return false;
}

bool StInitialization::OnEvent(const std::shared_ptr<smachine::Event> event){

    return false;
}

}//namespace weather