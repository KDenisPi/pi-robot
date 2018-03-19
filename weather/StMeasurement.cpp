/*
 * StMeasurement.cpp
 * 
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#include "StMeasurement.h"
#include "context.h"
#include "defines.h"

#include "Si7021.h"
#include "sgp30.h"
#include "bmp280.h"
#include "tsl2561.h"

namespace weather {

//take measurement from all sensors
void StMeasurement::measure(){

    auto context = get_env<weather::Context>();

    //make measurement using Si7021 and then use this values for SGP30
    auto si7021 = get_item<pirobot::item::Si7021>("SI7021");
    si7021->get_results(context->si7021_humidity, context->si7021_temperature, context->si7021_abs_humidity);

    auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
    sgp30->get_results(context->spg30_co2, context->spg30_tvoc);    

    auto bmp280 = get_item<pirobot::item::Bmp280>("BMP280");
    bmp280->get_results(context->bmp280_pressure, context->bmp280_temperature, context->bmp280_altitude);
}


void StMeasurement::OnEntry(){
    measure();

   TIMER_CREATE(TIMER_UPDATE_INTERVAL, 30) //wait for 15 seconds before real use
}

bool StMeasurement::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            get_itf()->finish();
            return true;
        }

        //switch to main state
        case TIMER_UPDATE_INTERVAL:
        {
            measure();
            return true;
        }
    }

    return false;
}

bool StMeasurement::OnEvent(const std::shared_ptr<smachine::Event> event){

    return false;
}

}//namespace weather