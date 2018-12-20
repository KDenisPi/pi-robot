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

    //Switch all OFF
    ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(new pirobot::item::OffTransformation())));
    //Switch one LED Red On
    ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(new pirobot::item::SetColorTransformation(0x00200000, 0, 1))));
    ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(new pirobot::item::SetColorTransformation(0x00002000, 3, 1))));
    //Switch LED
    ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(new pirobot::item::ShiftTransformation(25))));
    ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(new pirobot::item::ShiftTransformation(25, pirobot::item::Direction::ToLeft))));


    //get_itf()->timer_start(TIMER_FINISH_ROBOT, 30);
    get_itf()->timer_start(TIMER_USER_1, 40);
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
                ctrl->add_transformation( std::make_pair(std::string(), std::shared_ptr<pirobot::item::SledTransformer>(new pirobot::item::OffTransformation())));
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
