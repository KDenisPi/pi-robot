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

/*
    TODO:
        1. Deep switch off for LCD
        2. Initialize LCD after wake-up
        3. Switch LCD on if important event detected

*/

namespace weather {

void StMeasurement::headlights(const bool light_on){
    auto led_white_r = get_item<pirobot::item::Led>("led_white_r");
    auto led_white_l = get_item<pirobot::item::Led>("led_white_l");

    if(light_on){
        led_white_r->On();
        led_white_l->On();
    }
    else{
        led_white_r->Off();
        led_white_l->Off();
    }
}


//
// Take measurement from all sensors
//
void StMeasurement::measure(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto ctxt = get_env<weather::Context>();

    try {

        headlights(true);

        TIMER_CREATE(TIMER_MEASURE_LIGHT_INTERVAL, ctxt->measure_light_interval) //measurement interval

        Measurement data;

        //make measurement using Si7021 and then use this values for SGP30
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- SI7021");
        auto si7021 = get_item<pirobot::item::Si7021>("SI7021");
        si7021->get_results(data.si7021_humidity, data.si7021_temperature, data.si7021_abs_humidity);

        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- SGP30");
        auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
        sgp30->get_results(data.spg30_co2, data.spg30_tvoc);

        //update absolute humidity value
        sgp30->set_humidity(data.si7021_abs_humidity);

        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- BMP280");
        auto bmp280 = get_item<pirobot::item::Bmp280>("BMP280");
        bmp280->get_results(data.bmp280_pressure, data.bmp280_temperature, data.bmp280_altitude);

        //
        //detect luminosity and of light was changed from On to Off or from Off to On - create event
        //
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ----- TSL2561");
        auto tsl2561 = get_item<pirobot::item::Tsl2561>("TSL2561");
        int32_t tsl2651_lux = ctxt->data.tsl2651_lux;

        if( !tsl2561->get_results(data.tsl2651_lux)){
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " TSL2561 - Incorrect value detected");
            //if value is incorrect - ignore it
            data.tsl2651_lux = tsl2651_lux;
        }

        int lux_diff = data.tsl2651_lux - tsl2651_lux;

        float mdiff = 0.0;
        if(data.tsl2651_lux != 0 && tsl2651_lux!=0)
            mdiff = m_abs(m_change(data.tsl2651_lux, tsl2651_lux));

        if( mdiff > 2){
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " ----- TSL2561 --- Old: " + std::to_string(tsl2651_lux) +
                " New: " + std::to_string(data.tsl2651_lux) + " Diff: " + std::to_string(m_abs(lux_diff)) + " LhDiff:" + std::to_string(ctxt->light_off_on_diff) +
                " MDiff: " + std::to_string(mdiff));

            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            if( lux_diff>0 ){ //light On - LCD ON
                if( !lcd->is_on() ){
                    EVENT(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_USER, EVT_LCD_ON));
                }
            }
            else { //light OFF - LCD OFF
                if( lcd->is_on() ){
                    EVENT(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_USER, EVT_LCD_OFF));
                }
            }
        }
        else if( data.tsl2651_lux < ctxt->light_low_level) { //Dark in the room - LCD OFF
            logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " ----- TSL2561 --- Old: " + std::to_string(tsl2651_lux) +
                " New: " + std::to_string(data.tsl2651_lux) + " Diff: " + std::to_string(m_abs(lux_diff)) + " LhDiff:" + std::to_string(ctxt->light_off_on_diff) +
                " MDiff: " + std::to_string(mdiff));

            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            if( lcd->is_on() ){
                EVENT(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_USER, EVT_LCD_OFF));
            }
        }

        //update measurement information
        ctxt->data = data;

        int co2_lvl = ctxt->get_CO2_level();
        int tvoc_lvl = ctxt->get_TVOC_level();

        // update CO2 & TVOC levels
        ctxt->update_CO2_TVOC_levels();

        //Level switch from Low to High
        if((co2_lvl<4 && tvoc_lvl<4) && (ctxt->get_CO2_level()==4 || ctxt->get_TVOC_level()==4)){
            EVENT(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_USER, EVT_HIGH_LEVEL_ON));
        }

        //Level switch from High to Low
        if((co2_lvl==4 || tvoc_lvl==4) && (ctxt->get_CO2_level()<4 && ctxt->get_TVOC_level()<4)){
            EVENT(std::make_shared<smachine::Event>(smachine::EVENT_TYPE::EVT_USER, EVT_HIGH_LEVEL_OFF));
        }

    }
    catch(std::runtime_error& exc){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + exc.what());
    }
    catch(std::exception& exc){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + exc.what());
    }
    catch(...){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Unknown exception");
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
}

/*
*
*/
bool StMeasurement::storage_start(){
    auto ctxt = get_env<weather::Context>();
    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Local time: " + std::to_string(ctxt->_fstor_local_time));

    if(!ctxt->start_file_storage()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not initialize file storage");
        TIMER_CREATE(TIMER_FINISH_ROBOT, 5);
        return false;
    }

    if(!ctxt->start_mqtt_storage()){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not initialize MQTT storage");
        TIMER_CREATE(TIMER_FINISH_ROBOT, 5);
        return false;
    }

#ifdef USE_SQL_STORAGE
    if(!ctxt->_sqlstorage.start(ctxt->_db_name)){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not initialize SQL storage");
        TIMER_CREATE(TIMER_FINISH_ROBOT, 5);
        return false;
    }
#endif

    return true;
}

/*
*
*/
bool StMeasurement::storage_stop(){
    auto ctxt = get_env<weather::Context>();

    ctxt->stop_file_storage();
    ctxt->stop_mqtt_storage();

#ifdef USE_SQL_STORAGE
    ctxt->_sqlstorage.stop();
#endif

    return true;
}

/*
* Write measurement result to storage(s)
*/
void StMeasurement::storage_write(Measurement& meas){
    auto ctxt = get_env<weather::Context>();
    ctxt->write_measurement(meas);
}

//
// State entry function
//
void StMeasurement::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    //
    //Initialize storages
    //
    if( !storage_start()){
        return;
    }

    auto ctxt = get_env<weather::Context>();
    //initial measurement
    measure();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Create Timer ID: " + std::to_string(TIMER_UPDATE_INTERVAL));

    //Create timers
    TIMER_CREATE(TIMER_UPDATE_INTERVAL, ctxt->measure_check_interval) //measurement interval
    TIMER_CREATE(TIMER_SHOW_DATA_INTERVAL, ctxt->measure_show_interval) //update measurement information on LCD interval
    TIMER_CREATE(TIMER_WRITE_DATA_INTERVAL, ctxt->measure_write_interval) //save information
}

//
// Process Timer events
//
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

        case TIMER_WRITE_DATA_INTERVAL:
        {
            //Save result to file

            //
            //TODO: Add MQTT here
            //
            auto ctxt = get_env<weather::Context>();
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Write measurement");

            Measurement data = ctxt->data;
            storage_write(data);

            TIMER_CREATE(TIMER_WRITE_DATA_INTERVAL, ctxt->measure_write_interval) //save information
        }

        case TIMER_SHOW_DATA_INTERVAL:
        {
            //
            //Update measurement on LCD screen
            //
            update_lcd();
            return true;
        }

        case TIMER_MEASURE_LIGHT_INTERVAL:
        {
            headlights(false);
            return true;
        }
    }

    return false;
}

//
// Update informationon LCD screen
//
void StMeasurement::update_lcd(){
    auto ctxt = get_env<weather::Context>();
    auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");

    //Check if LCD switched off - do nothing
    if(!lcd->is_on()){
        return;
    }

    auto str_0 = ctxt->measure_view(0);
    auto str_1 = ctxt->measure_view(1);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Line 0: " + str_0 + " Line 1: " + str_1);

    lcd->write_string_at(0,0, str_0, true);
    lcd->write_string_at(1,0, str_1, false);

    //update measurement information on LCD interval
    TIMER_CREATE(TIMER_SHOW_DATA_INTERVAL, ctxt->measure_show_interval)
}

bool StMeasurement::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnEvent: " + event->to_string());

    auto ctxt = get_env<weather::Context>();

    if(smachine::EVENT_TYPE::EVT_USER == event->type()){
        //High level CO2 or TVOC detected
        if(event->name() == EVT_HIGH_LEVEL_ON){
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " High level detected!!");

            //
            // Always switch LCD to ON
            //
            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
            lcd->On();

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

            //
            //Update measurement on LCD screen
            //
            update_lcd();

            return true;
        }

        //light was switched On or Off
        if(event->name() == EVT_LCD_ON || event->name() == EVT_LCD_OFF){
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Event: " + event->name());

            auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");

            //
            //Switch LCD OFF
            //
            if(event->name() == EVT_LCD_OFF && lcd->is_on()){
                //Stop update LCD timer
                TIMER_CANCEL(TIMER_SHOW_DATA_INTERVAL);
                lcd->Off();
            }

            //
            //Switch LCD ON
            //
            if(event->name() == EVT_LCD_ON && !lcd->is_on()){
                lcd->On();
                update_lcd();
            }
            return true;
        }
    }

    return false;
}

//
// Stop measurement and save current state
//
void StMeasurement::finish(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

    auto ctxt = get_env<weather::Context>();
    auto lcd = get_item<pirobot::item::lcd::Lcd>("Lcd");
    lcd->write_string_at(0,0, ctxt->get_str(StrID::Finishing), true);

    TIMER_CANCEL(TIMER_UPDATE_INTERVAL);
    TIMER_CANCEL(TIMER_IP_CHECK_INTERVAL);
    TIMER_CANCEL(TIMER_SHOW_DATA_INTERVAL);
    TIMER_CANCEL(TIMER_WRITE_DATA_INTERVAL);

    auto context = get_env<weather::Context>();
    //Stop SGP30 and save current values
    auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
    sgp30->stop();

    sgp30->get_baseline(context->data.spg30_base_co2, context->data.spg30_base_tvoc);
    std::string data_file = "./initial.json";
    context->save_initial_data(data_file);

    lcd->clear_display();
    lcd->Off();

    //Stop all used storages
    storage_stop();
}

}//namespace weather
