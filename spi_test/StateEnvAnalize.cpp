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

#include "sled.h"
#include "sledctrl.h"

namespace spi_test {
namespace state {

const char TAG[] = "SLed";

StateEnvAnalize::StateEnvAnalize(smachine::StateMachineItf* itf) :
        smachine::state::State(itf, "StateEnvAnalize")
{
    // TODO Auto-generated constructor stub
}

StateEnvAnalize::~StateEnvAnalize() {
    // TODO Auto-generated destructor stub
}

#define NOP(obj, evt) LAST_OP(obj, evt, StateEnvAnalize::ntf_finished, this)

void StateEnvAnalize::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize started");

    auto ctrl = get_item<pirobot::item::sledctrl::SLedCtrl>("SLedCtrlSpi");
    add_transformations();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize finished");
}

/*
*
*/
void StateEnvAnalize::add_transformations(){
    auto ctrl = get_item<pirobot::item::sledctrl::SLedCtrl>("SLedCtrlSpi");

    ctrl->transformations_clear();

    std::vector<uint32_t> rgbs;
    for( int i = 0; i < 32; i++){
        rgbs.push_back(( (i%2) == 0 ? _rand() : 0x00000000));
    }

    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_r_shift(32);
    ctrl->add_l_shift(32);

    for( int i = 0; i < 32; i++){
        rgbs.push_back(( (i%2) == 0 ? 0x00AA0000 : 0x00000000));
    }
    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_r_shift(32);
    ctrl->add_l_shift(32);

    for( int i = 0; i < 32; i++){
        rgbs.push_back(( (i%2) == 0 ? _rand() : 0x00000000));
    }

    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_r_shift(32);
    ctrl->add_l_shift(32);

    for( int i = 0; i < 32; i++){
        rgbs.push_back(( (i%2) == 0 ? 0x0000AA00 : 0x00000000));
    }
    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_r_shift(32);

    for( int i = 0; i < 32; i++){
        rgbs.push_back(( (i%2) == 0 ? 0x000000AA : 0x00000000));
    }
    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_l_shift(32);

    rgbs = {0x00202020 , 0x00000020 , 0x00200000, 0x00000000 ,
            0x00251010 , 0x00101020 , 0x00102510, 0x00000000 ,
            0x00150505 , 0x00050515 , 0x00051505, 0x00000000
            };
    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_r_shift(32);

    rgbs = {
            0x00150505 , 0x00050515 , 0x00051505, 0x00000000,
            0x00251010 , 0x00101020 , 0x00102510, 0x00000000,
            0x00202020 , 0x00000020 , 0x00200000, 0x00000000
            };
    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_l_shift(32);

    for( int i = 0; i < 32; i++){
        rgbs.push_back(( (i%2) == 0 ? 0x00cc2233 : 0x00000000));
    }
    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_r_shift(32);
    ctrl->add_l_shift(32);

    for( int i = 0; i < 32; i++){
        rgbs.push_back(( (i%2) == 0 ? _rand() : 0x00000000));
    }
    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_r_shift(32);
    ctrl->add_l_shift(32);

    for( int i = 0; i < 32; i++){
        rgbs.push_back(( (i%2) == 0 ? 0x0022cc33 : 0x00000000));
    }
    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_r_shift(32);

    for( int i = 0; i < 32; i++){
        rgbs.push_back(( (i%2) == 0 ? 0x002233cc : 0x00000000));
    }
    ctrl->add_copy_rgbs(rgbs);
    ctrl->add_l_shift(32);

    const std::function<void(const std::string&)> ffinished = std::bind(&StateEnvAnalize::ntf_finished, this, std::placeholders::_1);
    ctrl->add_last_operation(EVT_CYCLE_FINISHED, ffinished);

    ctrl->transformations_restart();
}

/*
*
*/
bool StateEnvAnalize::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    auto ctrl = get_item<pirobot::item::sledctrl::SLedCtrl>("SLedCtrlSpi");
    switch(id){
        case TIMER_FINISH_ROBOT:
            get_itf()->finish();
            return true;

        case TIMER_USER_1:
            {
                ctrl->add_led_off();

                logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Run stop timer 5 sec");
                get_itf()->timer_start(TIMER_FINISH_ROBOT, 5);

            }
            return true;
    }

    return false;
}

#define MAX_CYCLES 20

bool StateEnvAnalize::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ==== OnEvent Type: " + std::to_string(event->type()) +
            " Name:[" + event->name() + "]");

    if(smachine::EVENT_TYPE::EVT_USER == event->type()){
        /*
        * Cycle finished
        */
        if(event->name() == EVT_CYCLE_FINISHED){
            auto ctrl = get_item<pirobot::item::sledctrl::SLedCtrl>("SLedCtrlSpi");
            ctrl->transformations_restart();

            return true;
        }
    }

    return false;
}

} /* namespace state */
} /* namespace project1 */
