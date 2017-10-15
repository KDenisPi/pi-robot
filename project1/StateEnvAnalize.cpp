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
#include "AnalogLightMeter.h"

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

    /*
    * 1. Switch both lightmetters On
    * 2. Set direction for DC motor counter clockwise 
    * 3. Switch DC motor On
    * 4. Switch Blue blink On
    */
    auto blue = get_item<pirobot::item::Blinking<pirobot::item::Led>>("BLINK_Blue");
    auto dcm_1 = get_item<pirobot::item::dcmotor::DCMotor>("DCM_1");
    auto lght_meter_1 = get_item<pirobot::anlglightmeter::AnalogLightMeter>("LightMeter_1");
    auto lght_meter_2 = get_item<pirobot::anlglightmeter::AnalogLightMeter>("LightMeter_2");
    
    dcm_1->set_direction(pirobot::item::MOTOR_DIR::DIR_COUTERCLOCKWISE);

    lght_meter_1->activate();
    lght_meter_2->activate();

    blue->On();
    dcm_1->set_power_level(5.0f);

/*
    auto step1 = std::static_pointer_cast<pirobot::item::ULN2003StepperMotor>(get_itf()->get_robot()->get_item("STEP_1"));
    step1->stop();
    step1->set_direction(pirobot::item::MOTOR_DIR::DIR_COUTERCLOCKWISE);
    step1->set_steps(600);
    */

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize finished");
}

bool StateEnvAnalize::OnTimer(const int id){
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
bool StateEnvAnalize::OnEvent(const std::shared_ptr<smachine::Event> event){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ==== OnEvent Type: " + std::to_string(event->type()) +
            " Name:[" + event->name() + "]");

    //emergency stop
    if(event->is_event("BTN_Stop")){
        if(event->type() == smachine::EVENT_TYPE::EVT_BTN_DOWN){
            get_itf()->finish();
            return true;
        }
    }

    if(event->is_event("LightMeter_1")){
        if(event->type() == smachine::EVENT_TYPE::EVT_LM_HIGH){
            return true;
        }            

        auto env = get_env<MyEnv>();
        auto dcm_1 = get_item<pirobot::item::dcmotor::DCMotor>("DCM_1");

        if(dcm_1->get_direction() == pirobot::item::MOTOR_DIR::DIR_COUTERCLOCKWISE){
            if(env->is_sensor_not_set(LM_SENSOR_0)){
                env->set_lm_time(LM_SENSOR_0);
            }
        }
        else{
            dcm_1->stop();
            auto yellow = get_item<pirobot::item::Blinking<pirobot::item::Led>>("BLINK_Yellow");
            yellow->Off();

            if(env->is_sensor_not_set(LM_SENSOR_1)){
                dcm_1->set_direction(pirobot::item::MOTOR_DIR::DIR_COUTERCLOCKWISE);
                dcm_1->set_power_level(5.0f);
                auto blue = get_item<pirobot::item::Blinking<pirobot::item::Led>>("BLINK_Blue");
                blue->On();
            }
        }
        return true;
    }

    if(event->is_event("LightMeter_2")){
        if(event->type() == smachine::EVENT_TYPE::EVT_LM_HIGH){
            return true;
        }            

        auto env = get_env<MyEnv>();
        auto dcm_1 = get_item<pirobot::item::dcmotor::DCMotor>("DCM_1");

        if(dcm_1->get_direction() == pirobot::item::MOTOR_DIR::DIR_CLOCKWISE){
            if(env->is_sensor_not_set(LM_SENSOR_1)){
                env->set_lm_time(LM_SENSOR_1);
            }
        }
        else{
            dcm_1->stop();
            auto blue = get_item<pirobot::item::Blinking<pirobot::item::Led>>("BLINK_Blue");
            blue->Off();

            if(env->is_sensor_not_set(LM_SENSOR_0)){
                dcm_1->set_direction(pirobot::item::MOTOR_DIR::DIR_CLOCKWISE);
                dcm_1->set_power_level(5.0f);
                auto yellow = get_item<pirobot::item::Blinking<pirobot::item::Led>>("BLINK_Yellow");
                yellow->On();
            }
        }
        return true;
    }

    return false;
}

} /* namespace state */
} /* namespace project1 */
