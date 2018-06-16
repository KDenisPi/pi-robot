/*
 * StInitializeSensors.cpp
 *
 *  Created on: June 01, 2018
 *      Author: Denis Kudia
 */

#include "StInitializeSensors.h"
#include "context.h"

#include "Si7021.h"
#include "sgp30.h"
#include "lcd.h"

namespace weather {

void StInitializeSensors::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto ctxt = get_env<weather::Context>();
    auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");

    lcd->write_string_at(0,0, ctxt->get_str(StrID::Warming), true);

    //load initial data
    std::string data_file = "./initial.json";
    ctxt->load_initial_data(data_file);

    //make measurement using Si7021 and then use this values for SGP30
    auto si7021 = get_item<pirobot::item::Si7021>("SI7021");
    si7021->get_results(ctxt->si7021_humidity, ctxt->si7021_temperature, ctxt->si7021_abs_humidity);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Result Humidity: " + std::to_string(ctxt->si7021_humidity) +
        " Temperature: " + std::to_string(ctxt->si7021_temperature) + "C");

    //use absolute humidity for SGP30
    auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
    sgp30->set_humidity(ctxt->si7021_abs_humidity);
    sgp30->set_baseline(ctxt->spg30_base_co2, ctxt->spg30_base_tvoc);
    sgp30->start();

   TIMER_CREATE(TIMER_WARM_INTERVAL, 15) //wait for 15 seconds before real use
}

bool StInitializeSensors::OnTimer(const int id){
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
            POP_STATE();
            return true;
        }
    }

    return false;
}

bool StInitializeSensors::OnEvent(const std::shared_ptr<smachine::Event> event){

    return false;
}

}//namespace weather
