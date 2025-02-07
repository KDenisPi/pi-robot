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
#define m_change(x,y) (x>y ? x/y : y/x)

class StMeasurement : public smachine::state::State {
public:
    StMeasurement() : smachine::state::State("StMeasurement") {}
    virtual ~StMeasurement(){}

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;

    //take measurement from all sensors
    void measure();
    //stop measurement and save current state
    void save_data_and_finish();
    //update information on LCD screen
    void update_lcd();

    //
    bool storage_start();
    bool storage_stop();
    void storage_write(Measurement& meas);

    //
    void headlights(const bool light_on);

private:

};

}//weather namespace
#endif
