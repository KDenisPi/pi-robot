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
#include "DataStorage.h"

namespace weather {

#define m_abs(x) (x>=0 ? x : -x)
#define m_change(x,y) (x>y ? x/y : y/x)

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

    //
    bool storage_start();
    bool storage_stop();
    void storage_write(Measurement& meas);

private:
    // File based data storage
#ifdef USE_FILE_STORAGE    
    weather::data::FileStorage _fstorage;
#endif

#ifdef USE_SQL_STORAGE    
    weather::data::SqlStorage _sqlstorage;
#endif    
};

}//weather namespace
#endif
