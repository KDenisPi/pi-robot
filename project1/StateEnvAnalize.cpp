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

#include "led.h"
#include "blinking.h"
#include "DCMotor.h"
#include "ULN2003StepperMotor.h"
//#include "TiltSwitch.h"

namespace project1 {
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

    auto blue = std::static_pointer_cast<pirobot::item::Blinking<pirobot::item::Led>>(get_itf()->get_robot()->get_item("BLINK_Blue"));

    auto red    = std::static_pointer_cast<pirobot::item::Led>(get_itf()->get_robot()->get_item("LED_Red"));
    //auto blue   = std::static_pointer_cast<pirobot::item::Led>(get_itf()->get_robot()->get_item("LED_Blue"));
    auto yellow = std::static_pointer_cast<pirobot::item::Led>(get_itf()->get_robot()->get_item("LED_Yellow"));

    auto dcm1 = std::static_pointer_cast<pirobot::item::dcmotor::DCMotor>(get_itf()->get_robot()->get_item("DCM_1"));
    auto step1 = std::static_pointer_cast<pirobot::item::ULN2003StepperMotor>(get_itf()->get_robot()->get_item("STEP_1"));

    step1->set_direction(pirobot::item::MOTOR_DIR::DIR_CLOCKWISE);
    step1->set_steps(600);
    /*
    step1->stop();
    step1->set_direction(pirobot::item::MOTOR_DIR::DIR_COUTERCLOCKWISE);
    step1->set_steps(600);
    */

    red->On();
    blue->On();
    yellow->On();

    //dcm1->set_power_level(5.0f);

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize finished");
}

bool StateEnvAnalize::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer started ID: " + std::to_string(id));

/*
    switch(id){
    case TIMER_FINISH_ROBOT:
        get_itf()->finish();
        return true;
    }
*/
    return false;
}

bool StateEnvAnalize::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ==== OnEvent Type: " + std::to_string(event->type()) +
            " Name:[" + event->name() + "]");

    if(event->is_event("BTN_Stop")){
        if(event->type() == smachine::EVENT_TYPE::EVT_BTN_DOWN){
            get_itf()->finish();
            return true;
        }
    }

    if(event->is_event("BLINK_Blue")){
      auto red    = std::static_pointer_cast<pirobot::item::Led>(get_itf()->get_robot()->get_item("LED_Red"));
      auto yellow = std::static_pointer_cast<pirobot::item::Led>(get_itf()->get_robot()->get_item("LED_Yellow"));

      yellow->Off();
      red->Off();

      //auto dcm1 = std::static_pointer_cast<pirobot::item::dcmotor::DCMotor>(get_itf()->get_robot()->get_item("DCM_1"));
      //dcm1->stop();

      auto step1 = std::static_pointer_cast<pirobot::item::ULN2003StepperMotor>(get_itf()->get_robot()->get_item("STEP_1"));
      step1->stop_rotation();

      return true;
    }

    return false;
}

} /* namespace state */
} /* namespace project1 */
