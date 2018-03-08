/*
 * StateNoHardware.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: Denis Kudia
 */

#include <vector>

#include "defines.h"
#include "StateWeather.h"
#include "MyEnv.h"
#include "Si7021.h"
#include "sgp30.h"
#include "bmp280.h"
#include "tsl2561.h"

namespace project1 {
namespace state {

StateWeather::StateWeather(smachine::StateMachineItf* itf) :
        smachine::state::State(itf, "StateWeather")
{
    // TODO Auto-generated constructor stub

}

StateWeather::~StateWeather() {
    // TODO Auto-generated destructor stub
}


void StateWeather::get_spg30_values(){
    uint16_t co2, tvoc, bs_co2, bs_tvoc;

    auto sgp30 = get_item<pirobot::item::Sgp30>("SGP30");
    sgp30->get_results(co2, tvoc);
    sgp30->get_baseline(bs_co2, bs_tvoc);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Result CO2: " + std::to_string(co2) + " TVOC: " + std::to_string(tvoc));
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Baseline CO2: " + std::to_string(bs_co2) + " TVOC: " + std::to_string(bs_tvoc));
}

void StateWeather::get_bmp280_values(){
    float pressure, temp;

    auto bmp280 = get_item<pirobot::item::Bmp280>("BMP280");
    bmp280->get_results(pressure, temp);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Pressure: " + std::to_string(pressure)+ "Pa (" +  std::to_string(pressure/133.3) + "mmHg) Temperature: " + std::to_string(temp) + "C");
}

void StateWeather::get_tsl2561_values(){
    uint32_t lux;

    auto tsl2561 = get_item<pirobot::item::Tsl2561>("TSL2561");
    bool success = tsl2561->get_results(lux);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Lux: " + std::to_string(lux)+ " Overflow: " + std::to_string(!success));
}


void StateWeather::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateWeather started");

    TIMER_CREATE(TIMER_FINISH_ROBOT, 40)
    TIMER_CREATE(TIMER_GET_VALUE, 15)

    //auto si7021 = get_item<pirobot::item::Si7021>("Si7021");
    //si7021->measurement();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateWeather finished");
}

bool StateWeather::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
            get_itf()->finish();
            return true;
        case TIMER_GET_VALUE:
        {
            //get_bmp280_values();
            get_tsl2561_values();
            
            TIMER_CREATE(TIMER_GET_VALUE, 5)
        }
    }
    return false;
}

/*
* TODO: Move processing for BTN stop to common level
*/
bool StateWeather::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ==== OnEvent Type: " + std::to_string(event->type()) +
            " Name:[" + event->name() + "]");

    return false;
}

} /* namespace state */
} /* namespace project1 */
