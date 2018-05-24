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

#define MEASURE_INTERVAL 10
#define RUN_INTERVAL 35

//take measurement from all sensors
void StMeasurement::measure(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto context = get_env<weather::Context>();

    //make measurement using Si7021 and then use this values for SGP30
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- SI7021");
    auto si7021 = get_item<pirobot::item::Si7021>("SI7021");
    si7021->get_results(context->si7021_humidity, context->si7021_temperature, context->si7021_abs_humidity);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- SGP30");
    auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
    sgp30->get_results(context->spg30_co2, context->spg30_tvoc);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- BMP280");
    auto bmp280 = get_item<pirobot::item::Bmp280>("BMP280");
    bmp280->get_results(context->bmp280_pressure, context->bmp280_temperature, context->bmp280_altitude);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
}


void StMeasurement::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    measure();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Create Timer ID: " + std::to_string(TIMER_UPDATE_INTERVAL));
    TIMER_CREATE(TIMER_UPDATE_INTERVAL, MEASURE_INTERVAL) //measurement interval

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Create Timer ID: " + std::to_string(TIMER_FINISH_ROBOT));
    TIMER_CREATE(TIMER_FINISH_ROBOT, RUN_INTERVAL) //stop interval (test purpose)
}

bool StMeasurement::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            finish();

            get_itf()->finish();
            return true;
        }

        //switch to main state
        case TIMER_UPDATE_INTERVAL:
        {
            measure();
            TIMER_CREATE(TIMER_UPDATE_INTERVAL, MEASURE_INTERVAL) //measurement interval
            return true;
        }
    }

    return false;
}

bool StMeasurement::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEvent: " + event->to_string());

    return false;
}

//stop measurement and save current state
void StMeasurement::finish(){
            TIMER_CANCEL(TIMER_UPDATE_INTERVAL);

            auto context = get_env<weather::Context>();
            //Stop SGP30 and save current values
            auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
            sgp30->stop();

            sgp30->get_baseline(context->spg30_base_co2, context->spg30_base_tvoc);
            std::string data_file = "./initial.json";
            context->save_initial_data(data_file);
}

}//namespace weather
