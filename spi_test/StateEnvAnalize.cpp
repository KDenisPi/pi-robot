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

#define SHIFT_R(n) ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(\
            new pirobot::item::ShiftTransformation(n))));
#define SHIFT_L(n) ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(\
            new pirobot::item::ShiftTransformation(n, pirobot::item::Direction::ToLeft))));
#define LED_OFF()  ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(new pirobot::item::OffTransformation())));
#define SET_RGB(rgb,stpos) ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(\
            new pirobot::item::SetColorTransformation(rgb, stpos, 1))));
#define SET_RGBS(n) ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(\
            new pirobot::item::SetColorGroupTransformation(n))));
#define NOP(evt) ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(\
            new pirobot::item::NopTransformation(evt,std::bind(&StateEnvAnalize::ntf_finished, this, std::placeholders::_1)))));

void StateEnvAnalize::OnEntry(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize started");

    auto ctrl = get_item<pirobot::item::sledctrl::SLedCtrl>("SLedCtrl");

    //Switch all OFF
    LED_OFF()

    //Switch one LED Red On
    SET_RGB(0x00200000, 0)
    SHIFT_R(32)

    SET_RGB(0x00002000, 3)
    SHIFT_R(32)

    LED_OFF()

    std::array<uint32_t, 5> rgbs = {0x00202020 /*White*/, 0x00000020 /*Blue*/, 0x00200000/*Red*/};
    SET_RGBS(rgbs)

    SHIFT_R(32)
    SHIFT_L(32)

    NOP(EVT_CYCLE_FINISHED)

    //get_itf()->timer_start(TIMER_FINISH_ROBOT, 30);
    //get_itf()->timer_start(TIMER_USER_1, 120);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize finished");
}

bool StateEnvAnalize::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    auto ctrl = get_item<pirobot::item::sledctrl::SLedCtrl>("SLedCtrl");
    switch(id){
        case TIMER_FINISH_ROBOT:
            get_itf()->finish();
            return true;

        case TIMER_USER_1:
            {
                LED_OFF()

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

    if(smachine::EVENT_TYPE::EVT_USER == event->type()){
        /*
        * Cycle finished
        */
        if(event->name() == EVT_CYCLE_FINISHED){
            get_itf()->timer_start(TIMER_FINISH_ROBOT, 5);
        }
    }

    return false;
}

} /* namespace state */
} /* namespace project1 */
