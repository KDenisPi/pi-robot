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

namespace weather {

void StInitializeLcd::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
    lcd->display_ctrl(LCD_DISPLAYON|LCD_CURSORON|LCD_BLINKOFF);

    auto led_red = get_item<pirobot::item::Led>("led_red");
    auto led_green = get_item<pirobot::item::Led>("led_green");
    auto led_white = get_item<pirobot::item::Led>("led_white");
    led_red->On();
    led_green->On();
    led_white->On();

    //CHANGE_STATE("StMeasurement");
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Display On, Cursor On, Blink On");
    TIMER_CREATE(TIMER_LCD_INTERVAL, 2) //wait for 15 seconds before real use
}

bool StInitializeLcd::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
        {
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer FINISH_ROBOT");

            auto led_red = get_item<pirobot::item::Led>("led_red");
            auto led_green = get_item<pirobot::item::Led>("led_green");
            auto led_white = get_item<pirobot::item::Led>("led_white");
            led_red->Off();
            led_green->Off();
            led_white->Off();

            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            lcd->clear_display();
            lcd->display_off();
            lcd->backlight_off();

            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer FINISH_ROBOT Finish");
            get_itf()->finish();
            return true;
        }
        case TIMER_LCD_INTERVAL:
        {
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer LCD_INTERVAL");
            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            //lcd->backlight_off();
            //lcd->cursor_blink_off();
            lcd->set_cursor(0,0);
            lcd->write_string("Hello Denis! Hello Natasha!");
            lcd->set_cursor(1,0);
            lcd->write_string("Hello Alina!");
            lcd->scroll_display_left(5);

            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timer LCD_INTERVAL Blink Off, Start FINISH_TIMER");
            TIMER_CREATE(TIMER_FINISH_ROBOT, 15)
            return true;
        }
    }

    return false;
}

bool StInitializeLcd::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    return false;
}

}//namespace weather
