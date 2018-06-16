/*
 * StMeasurement.cpp
 *
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#include "StMeasurement.h"
#include "defines.h"

#include "Si7021.h"
#include "sgp30.h"
#include "bmp280.h"
#include "tsl2561.h"
#include "lcd.h"
#include "led.h"
#include "blinking.h"

namespace weather {

//take measurement from all sensors
void StMeasurement::measure(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto ctxt = get_env<weather::Context>();

    auto led_green = get_item<pirobot::item::Led>("led_green");
    led_green->On();

    //make measurement using Si7021 and then use this values for SGP30
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- SI7021");
    auto si7021 = get_item<pirobot::item::Si7021>("SI7021");
    si7021->get_results(ctxt->si7021_humidity, ctxt->si7021_temperature, ctxt->si7021_abs_humidity);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- SGP30");
    auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
    sgp30->get_results(ctxt->spg30_co2, ctxt->spg30_tvoc);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- BMP280");
    auto bmp280 = get_item<pirobot::item::Bmp280>("BMP280");
    bmp280->get_results(ctxt->bmp280_pressure, ctxt->bmp280_temperature, ctxt->bmp280_altitude);

    int co2_lvl = ctxt->get_CO2_level();
    int tvoc_lvl = ctxt->get_TVOC_level();

    // update CO2 & TVOC levels
    ctxt->update_CO2_TVOC_levels();

    //Level switch from Low to High
    if((co2_lvl<4 && tvoc_lvl<4) && (ctxt->get_CO2_level()==4 || ctxt->get_TVOC_level()==4)){
        std::shared_ptr<smachine::Event> event(new smachine::Event(smachine::EVENT_TYPE::EVT_USER, EVT_HIGH_LEVEL_ON));
        EVENT(event);
    }

    //Level switch from High to Low
    if((co2_lvl==4 || tvoc_lvl==4) && (ctxt->get_CO2_level()<4 && ctxt->get_TVOC_level()<4)){
        std::shared_ptr<smachine::Event> event(new smachine::Event(smachine::EVENT_TYPE::EVT_USER, EVT_HIGH_LEVEL_OFF));
        EVENT(event);
    }

    led_green->Off();
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
}


void StMeasurement::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    measure();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Create Timer ID: " + std::to_string(TIMER_UPDATE_INTERVAL));
    auto ctxt = get_env<weather::Context>();

    TIMER_CREATE(TIMER_UPDATE_INTERVAL, ctxt->measure_check_interval) //measurement interval
    TIMER_CREATE(TIMER_SHOW_DATA_INTERVAL, ctxt->measure_show_interval) //update measurement information on LCD interval

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

            auto ctxt = get_env<weather::Context>();
            TIMER_CREATE(TIMER_UPDATE_INTERVAL, ctxt->measure_check_interval) //measurement interval

            return true;
        }

        case TIMER_SHOW_DATA_INTERVAL:
        {
            auto ctxt = get_env<weather::Context>();

            auto str_0 = measure_view(0);
            auto str_1 = measure_view(1);

            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Line 0: " + str_0 + " Line 1: " + str_1);

            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            lcd->write_string_at(0,0, str_0, true);
            lcd->write_string_at(1,0, str_1, false);

            TIMER_CREATE(TIMER_SHOW_DATA_INTERVAL, ctxt->measure_show_interval) //update measurement information on LCD interval
            return true;
        }
    }

    return false;
}

bool StMeasurement::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEvent: " + event->to_string());

    auto ctxt = get_env<weather::Context>();

    if(smachine::EVENT_TYPE::EVT_USER == event->type()){
        //High level CO2 or TVOC detected
        if(event->name() == EVT_HIGH_LEVEL_ON){
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " High level detected!!");

            //Start blink Red LED
            auto red_blinker = get_item<pirobot::item::Blinking<pirobot::item::Led>>("red_blinker");
            red_blinker->On();

            return true;
        }
        //Level CO2 or TVOC returned to normal
        if(event->name() == EVT_HIGH_LEVEL_OFF){
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Level returned to normal");

            //Stop blink Red LED
            auto red_blinker = get_item<pirobot::item::Blinking<pirobot::item::Led>>("red_blinker");
            red_blinker->Off();

            return true;
        }
    }

    return false;
}

//stop measurement and save current state
void StMeasurement::finish(){

            auto ctxt = get_env<weather::Context>();
            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            lcd->write_string_at(0,0, ctxt->get_str(StrID::Finishing), true);

            TIMER_CANCEL(TIMER_UPDATE_INTERVAL);
            TIMER_CANCEL(TIMER_IP_CHECK_INTERVAL);
            TIMER_CANCEL(TIMER_SHOW_DATA_INTERVAL);

            auto context = get_env<weather::Context>();
            //Stop SGP30 and save current values
            auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
            sgp30->stop();

            sgp30->get_baseline(context->spg30_base_co2, context->spg30_base_tvoc);
            std::string data_file = "./initial.json";
            context->save_initial_data(data_file);

            lcd->clear_display();
            lcd->display_off();
            lcd->backlight_off();
}

}//namespace weather
