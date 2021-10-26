/*
 * button.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: Denis Kudia
 */

#include <cassert>
#include <unistd.h>

#include "button.h"
#include "logger.h"


namespace pirobot {
namespace item {

const char TAG[] = "button";

const char* Item::ItemNames[] = {
    "UNKNOWN",
    "LED",
    "BUTTON",
    "SERVO",
    "TILT_SWITCH",
    "STEPPER",
    "DCMotor",
    "DRV8835",
    "BLINKER",
    "AnlgDgtConvertor",
    "AnalogLightMeter",
    "ULN2003Stepper",
    "MPU6050",
    "SI7021",
    "SGP30",
    "BMP280",
    "TSL2561",
    "LCD",
    "SLED",
    "SLEDCTRLSPI",
    "SLEDCTRLPWM",
    "AnalogMeterSimple",
    "DustSensor",
    "Tmp36"
};

/*
 *
 */
Button::Button(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
        const std::string name, const std::string comment,
        const BUTTON_STATE state, const gpio::PULL_MODE pullmode, const int itype) :
          Item(gpio, name, comment, itype), m_pullmode(pullmode), m_state(state)
{
    assert(get_gpio() != NULL);
    assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::IN);

    //not safety
    const std::shared_ptr<pirobot::gpio::Gpio> pgpio = get_gpio();

    inverse_value = (pgpio->get_provider_type() == gpio::GPIO_PROVIDER_TYPE::PROV_MCP23017 ? true : false);

    if(name.empty()){
        set_name(type_name(type())  + "_over_" + pgpio->to_string());
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started. " + to_string() + " Inverse:" + std::to_string(inverse_value) +
           " Provider: " + std::to_string(pgpio->get_provider_type()) + " GPIO:" + pgpio->to_string());
}

/*
 *
 */
Button::~Button() {
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " " + name());
}

/*
 *
 */
void Button::stop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " " + name());
    piutils::Threaded::stop();
}

/*
 *
 */
bool Button::initialize(void)
{
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " " + name() + " PullMode: " + std::to_string(m_pullmode));

    /*
     * Set PULL MODE
     */
    const std::shared_ptr<pirobot::gpio::Gpio> gpio = get_gpio();

    gpio->pullUpDnControl(m_pullmode);

    gpio::SGN_LEVEL level = gpio->get_level();
    set_state(get_state_by_level(level));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " " + this->to_string() + " Current level:" + std::to_string(level));

    if(gpio->get_edge_level() != gpio::GPIO_EDGE_LEVEL::EDGE_NONE){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " " + this->to_string() + " GPIO supported EGDE. Do not start thread");
        return piutils::Threaded::start<Button>(this);
    }

    return true;
}

/*
 *
 */
const std::string Button::to_string(){
    return name() + (m_state == BUTTON_STATE::BTN_PUSHED ? " state: PUSHED" : " state: NOT PUSHED");
}

/*
*
*/
const std::string Button::printConfig(){
        return name() + " GPIO: " + get_gpio()->to_string();
}

/*
 *
 */
void Button::set_state(const BUTTON_STATE state){
    logger::log(logger::LLOG::DEBUG, TAG, " State changed from: " + std::to_string(m_state) + " to: " + std::to_string(state));
    m_state = state;
}


/*
 *
 */
void Button::worker(Button* owner){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + "Worker started Item:" + owner->name() + " Initial State :" + std::to_string(owner->state()));

    gpio::SGN_LEVEL curr_level = owner->get_gpio()->get_level();
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Current level:" + std::to_string(curr_level));

    while(!owner->is_stop_signal()){
        gpio::SGN_LEVEL level = owner->get_gpio()->get_level();
        if(level != curr_level){
            owner->process_level(level);
            curr_level = level;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(owner->get_loop_delay()));
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished.");
}

} /* namespace item */
} /* namespace pirobot */
