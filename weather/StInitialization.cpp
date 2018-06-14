/*
 * StInitialization.cpp
 *
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#include "StInitialization.h"
#include "button.h"


namespace weather {

void StInitialization::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

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
                generate_button_press(event->name(), btn_pressed_duration);
            }
        }

        if(event->name() == btn2->name()){
            if(smachine::EVENT_TYPE::EVT_BTN_DOWN == event->type())
                _btn2_down = std::chrono::system_clock::now();
            else{
                _btn2_up = std::chrono::system_clock::now();
                int btn_pressed_duration = std::chrono::duration_cast<std::chrono::seconds>(_btn2_up -_btn2_down).count();
                generate_button_press(event->name(), btn_pressed_duration);
            }
        }

        return true;
    }

    return false;
}

//
//Process substite exit
//
void StInitialization::OnSubstateExit(const std::string substate_name) {
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started SubState: " + substate_name);

    if(substate_name == "StInitializeSensors"){
        CHANGE_STATE("StMeasurement");
    }
    else if(substate_name == "StInitializeLcd"){

        //detect IP address and save it for future using
        detect_ip_address();

        CHANGE_STATE("StInitializeSensors");
    }
}


}//namespace weather
