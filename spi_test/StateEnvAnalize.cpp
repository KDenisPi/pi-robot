/*
 * StateEnvAnalize.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: Denis Kudia
 */

#include <vector>

#include "defines.h"
#include "StateEnvAnalize.h"
#include "MyEnv.h"

#include "sledctrl.h"

namespace spi_test {
namespace state {

StateEnvAnalize::StateEnvAnalize(smachine::StateMachineItf* itf) :
        smachine::state::State(itf, "StateEnvAnalize")
{
    // TODO Auto-generated constructor stub

}

StateEnvAnalize::~StateEnvAnalize() {
    // TODO Auto-generated destructor stub
}

void StateEnvAnalize::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize started");

    auto ctrl = get_item<pirobot::item::sledctrl::SLedCtrl>("SLedCtrl");
    //ctrl->On();

    ctrl->color(0, (uint32_t)0x00000000, 0, 32);
    ctrl->color(0, (uint32_t)0x00550000, 0, 5);
    ctrl->refresh();
    ctrl->color(0, (uint32_t)0x0000FF00, 10, 15);
    ctrl->refresh();

    //get_itf()->timer_start(TIMER_FINISH_ROBOT, 5);
    get_itf()->timer_start(TIMER_USER_1, 1);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize finished");
}

bool StateEnvAnalize::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    auto ctrl = get_item<pirobot::item::sledctrl::SLedCtrl>("SLedCtrl");
    switch(id){
        case TIMER_FINISH_ROBOT:
            ctrl->color(0, (uint32_t)0x00000000);
            ctrl->refresh();

            //ctrl->Off();

            get_itf()->finish();
            return true;

        case TIMER_USER_1:
            {
                ctrl->color(0, (uint32_t)0x000000FF, 5, 10);
                ctrl->refresh();

                logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Run stop timer 5 sec");
                get_itf()->timer_start(TIMER_FINISH_ROBOT, 5);

            }
            return true;
    }

    return false;
}

bool StateEnvAnalize::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ==== OnEvent Type: " + std::to_string(event->type()) +
            " Name:[" + event->name() + "]");


    return false;
}

} /* namespace state */
} /* namespace project1 */
