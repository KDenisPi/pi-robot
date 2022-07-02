/*
 * StInitialization.cpp
 *
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#include "StInitialization.h"
#include "button.h"
#include "lcd.h"
#include "led.h"

namespace weather {

void StInitialization::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto ctxt = get_env<weather::Context>();

    /*
    * Initialize available storages (file, mqtt, sql)
    */

    ctxt->init_file_storage();
    ctxt->init_mqtt_storage();

    state_change("StInitializeLcd");
}

bool StInitialization::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            finish();
            return true;
        }
        break;
        //IP check interval
        case TIMER_IP_CHECK_INTERVAL:
        {
            //detect IP address and save it for future using
            detect_ip_address();

            auto ctxt = get_env<weather::Context>();
            timer_create(TIMER_IP_CHECK_INTERVAL, ctxt->ip_check_interval);
        }
        break;
    }

    return false;
}

bool StInitialization::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + "  Event: " + event->to_string());

    auto ctxt = get_env<weather::Context>();

    //Process button pressed
    if( (smachine::EVENT_TYPE::EVT_BTN_DOWN == event->type()) ||
          (smachine::EVENT_TYPE::EVT_BTN_UP == event->type())){
        auto btn1 = get_item<pirobot::item::Button>("btn_1");
        auto btn2 = get_item<pirobot::item::Button>("btn_2");

        //Save time moment when button was pressed
        if(event->name() == btn1->name()){
            if(smachine::EVENT_TYPE::EVT_BTN_DOWN == event->type())
                _btn1_down = std::chrono::system_clock::now();
            else{
                _btn1_up = std::chrono::system_clock::now();
                int btn_pressed_duration = std::chrono::duration_cast<std::chrono::seconds>(_btn1_up -_btn1_down).count();
                generate_button_press(event->name(), (btn_pressed_duration == 0 ? 1 : btn_pressed_duration));
            }
        }

        if(event->name() == btn2->name()){
            if(smachine::EVENT_TYPE::EVT_BTN_DOWN == event->type())
                _btn2_down = std::chrono::system_clock::now();
            else{
                _btn2_up = std::chrono::system_clock::now();
                int btn_pressed_duration = std::chrono::duration_cast<std::chrono::seconds>(_btn2_up -_btn2_down).count();
                generate_button_press(event->name(), (btn_pressed_duration==0 ? 1 : btn_pressed_duration));
            }
        }

        return true;
    }

    if(smachine::EVENT_TYPE::EVT_USER == event->type()){
        //Show current IP address value
        if(event->name() == EVT_SHOW_IP){
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " IP4: " + ctxt->ip4_address + " IP6: " + ctxt->ip6_address);

            //write  IP information on LCD
            //First line Header, second IP4 address
            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            lcd->write_string_at(0,0, ctxt->get_str(StrID::Ip4Address), true);
            lcd->write_string_at(1,0, ctxt->ip4_address, false);

            return true;
        }
    }

    return false;
}

//
//Process substite exit
//
void StInitialization::OnSubstateExit(const std::string substate_name) {
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started SubState: " + substate_name);

    if(substate_name == "StInitializeSensors"){
        state_change("StMeasurement");
    }
    else if(substate_name == "StInitializeLcd"){

        //detect IP address and save it for future using
        detect_ip_address();

        auto ctxt = get_env<weather::Context>();
        timer_create(TIMER_IP_CHECK_INTERVAL, ctxt->ip_check_interval);

        state_change("StInitializeSensors");
    }
}

}//namespace weather
