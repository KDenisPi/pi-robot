/*
 * StInitializeLcd.cpp
 * 
 *  Created on: June 01, 2018
 *      Author: Denis Kudia
 */

#include "StInitializeLcd.h"
#include "context.h"
#include "lcd.h"


namespace weather {

void StInitializeLcd::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
    lcd->display_ctrl(LCD_DISPLAYON|LCD_CURSORON|LCD_BLINKON);

    //CHANGE_STATE("StMeasurement");
   TIMER_CREATE(TIMER_LCD_INTERVAL, 30) //wait for 15 seconds before real use
}

bool StInitializeLcd::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            lcd->backlight_off();
            
            get_itf()->finish();
            return true;
        }
        case TIMER_LCD_INTERVAL:
        {
            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            lcd->cursor_blink_off();
            TIMER_CREATE(TIMER_FINISH_ROBOT, 30)
            return true;
        }
    }

    return false;
}

bool StInitializeLcd::OnEvent(const std::shared_ptr<smachine::Event> event){

    return false;
}

}//namespace weather
