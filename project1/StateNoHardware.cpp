/*
 * StateNoHardware.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: Denis Kudia
 */

#include <vector>

#include "defines.h"
#include "StateNoHardware.h"
#include "MyEnv.h"

namespace project1 {
namespace state {

StateNoHardware::StateNoHardware(smachine::StateMachineItf* itf) :
        smachine::state::State(itf, "StateNoHardware")
{
    // TODO Auto-generated constructor stub

}

StateNoHardware::~StateNoHardware() {
    // TODO Auto-generated destructor stub
}

void StateNoHardware::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateNoHardware started");

    TIMER_CREATE(TIMER_FINISH_ROBOT, 25)

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateNoHardware finished");
}

bool StateNoHardware::OnTimer(const int id){
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
bool StateNoHardware::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ==== OnEvent Type: " + std::to_string(event->type()) +
            " Name:[" + event->name() + "]");

    return false;
}

} /* namespace state */
} /* namespace project1 */
