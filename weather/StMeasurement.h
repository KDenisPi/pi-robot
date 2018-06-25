/*
 * StMeasurement.h
 *
 *  Created on: March 18, 2018
 *      Author: Denis Kudia
 */

#ifndef WEATHER_STMEASUREMENT_H_
#define WEATHER_STMEASUREMENT_H_

#include "StateMachine.h"
#include "context.h"

namespace weather {

#define m_abs(x) (x>=0 ? x : -x)

class StMeasurement : public smachine::state::State {
public:
    StMeasurement(smachine::StateMachineItf* itf) : smachine::state::State(itf, "StMeasurement") {}
    virtual ~StMeasurement(){}

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;

    //take measurement from all sensors
    void measure();
    //stop measurement and save current state
    void finish();
    //update information on LCD screen
    void update_lcd();

    //Prepare output string for measurement results
    const std::string measure_view(const int line=0){
        char buff[512];
        memset(buff, 0x00, sizeof(buff));

        auto ctxt = get_env<weather::Context>();

        if(line == 0){
            const int co2_level = ctxt->get_CO2_level();
            const std::string co2_label = ctxt->get_level_label(co2_level);

            if(co2_level < 4){
                float temp = ctxt->data.temp();
                sprintf(buff, ctxt->get_str(StrID::Line1).c_str(), temp,
                    (ctxt->show_temperature_in_celcius() ? 'C' : 'F'), co2_label.c_str());
            }
            else{
                sprintf(buff, co2_label.c_str(), "CO2");
            }
        }
        else if(line == 1){
            const int tvoc_level = ctxt->get_TVOC_level();
            const std::string tvoc_label = ctxt->get_level_label(tvoc_level);
            if(tvoc_level<4)
                sprintf(buff, ctxt->get_str(StrID::Line2).c_str(), ctxt->data.si7021_humidity, '%', ctxt->data.bmp280_pressure);
            else
                sprintf(buff, tvoc_label.c_str(), "TVOC");
        }

        std::string result(buff);
        return result;
    }
};

}//weather namespace
#endif
