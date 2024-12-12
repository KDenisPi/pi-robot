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
#include "led.h"

namespace weather {

void StInitializeSensors::OnEntry(){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    const auto ctxt = GET_ENV(weather::Context);
    auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");

    auto led_white_r = get_item<pirobot::item::Led>("led_white_r");
    auto led_white_l = get_item<pirobot::item::Led>("led_white_l");
    auto led_red = get_item<pirobot::item::Led>("led_red");

    led_white_r->On();
    led_white_l->On();
    led_red->On();

    lcd->write_string_at(0,0, ctxt->get_str(StrID::Warming), true);

    //load initial data
    std::string data_file = "./initial.json";
    ctxt->load_initial_data(data_file);

    //make measurement using Si7021 and then use this values for SGP30
    auto si7021 = get_item<pirobot::item::Si7021>("SI7021");
    si7021->get_results(ctxt->data.si7021_humidity, ctxt->data.si7021_temperature, ctxt->data.si7021_abs_humidity);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Result Humidity: " + std::to_string(ctxt->data.si7021_humidity) +
        " Temperature: " + std::to_string(ctxt->data.si7021_temperature) + "C");

    //use absolute humidity for SGP30
    auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
    sgp30->set_humidity(ctxt->data.si7021_abs_humidity);
    sgp30->set_baseline(ctxt->data.spg30_base_co2, ctxt->data.spg30_base_tvoc);
    sgp30->start();

    init_timer(TIMER_WARM_INTERVAL, 15, 0, false); //wait for 15 seconds before real use
}

bool StInitializeSensors::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            STM_TIMER_CANCEL(TIMER_WARM_INTERVAL);
            STM_FINISH();
            return true;
        }
        //switch to main state
        case TIMER_WARM_INTERVAL:
        {
            const auto ctxt = GET_ENV(weather::Context);

            auto led_white_r = get_item<pirobot::item::Led>("led_white_r");
            auto led_white_l = get_item<pirobot::item::Led>("led_white_l");
            auto led_red = get_item<pirobot::item::Led>("led_red");

            led_white_r->Off();
            led_white_l->Off();
            led_red->Off();

            STM_STATE_POP();
            return true;
        }
    }

    return false;
}

bool StInitializeSensors::OnEvent(const std::shared_ptr<smachine::Event> event){

    return false;
}

}//namespace weather
