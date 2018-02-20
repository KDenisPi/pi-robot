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

void StateWeather::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateWeather started");

    TIMER_CREATE(TIMER_FINISH_ROBOT, 10)

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateWeather finished");
}

bool StateWeather::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
            get_itf()->finish();
            return true;
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
