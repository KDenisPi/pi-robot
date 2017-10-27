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

#include "AnalogLightMeter.h"


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

    auto lght_meter_1 = get_item<pirobot::anlglightmeter::AnalogLightMeter>("LightMeter_1");
    auto lght_meter_2 = get_item<pirobot::anlglightmeter::AnalogLightMeter>("LightMeter_2");

    lght_meter_1->activate();
    lght_meter_2->activate();
    
    get_itf()->timer_start(TIMER_LIGHT_METER, 30);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " StateEnvAnalize finished");
}

bool StateEnvAnalize::OnTimer(const int id){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " OnTimer ID: " + std::to_string(id));

    switch(id){
        case TIMER_FINISH_ROBOT:
            get_itf()->finish();
            return true;
        case TIMER_LIGHT_METER:
            {
                auto lght_meter_1 = get_item<pirobot::anlglightmeter::AnalogLightMeter>("LightMeter_1");
                auto lght_meter_2 = get_item<pirobot::anlglightmeter::AnalogLightMeter>("LightMeter_2");
                lght_meter_1->deactivate();
                lght_meter_2->deactivate();

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
