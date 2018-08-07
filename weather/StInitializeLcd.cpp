/*
 * StInitializeLcd.cpp
 *
 *  Created on: June 01, 2018
 *      Author: Denis Kudia
 */

#include "StInitializeLcd.h"
#include "context.h"
#include "lcd.h"
#include "led.h"
#include "lcdstrings.h"

namespace weather {

void StInitializeLcd::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto ctxt = get_env<weather::Context>();
    auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");

    lcd->display_ctrl(LCD_DISPLAYON|LCD_CURSORON|LCD_BLINKOFF);
    lcd->write_string_at(0,0, ctxt->get_str(StrID::Starting), true);

    //switch Green led ON
    auto led_white_r = get_item<pirobot::item::Led>("led_white_r");
    led_white_r->On();

    TIMER_CREATE(TIMER_LCD_INTERVAL, 5) //wait for 5 seconds before real use
}

bool StInitializeLcd::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_LCD_INTERVAL:
        {
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer LCD_INTERVAL");

            auto led_white_r = get_item<pirobot::item::Led>("led_white_r");
            led_white_r->Off();

            POP_STATE();
            return true;
        }
    }

    return false;
}

bool StInitializeLcd::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + "  Event: " + event->to_string());
    return false;
}

}//namespace weather
