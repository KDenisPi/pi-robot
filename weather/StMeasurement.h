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

    //Prepare output string for measurement results
    const std::string measure_view(const int line=0){
        char buff[512];
        memset(buff, 0x00, sizeof(buff));

        auto ctxt = get_env<weather::Context>();

        if(line == 0){
            float temp = (ctxt->si7021_temperature + ctxt->bmp280_temperature)/2;
            float temp_f = ctxt->temp_C_to_F(temp);
            sprintf(buff, "T %.0fC (%.0fF)", temp, temp_f);
        }
        else if(line == 1){
            float hm = ctxt->si7021_humidity*100; //humidity in %
            sprintf(buff, "RH %.0f%c P %.0f mm Hg) ", hm, '%', ctxt->bmp280_pressure);

        }

        std::string result(buff);
        return result;
    }
};

}//weather namespace
#endif