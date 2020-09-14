/*
* dustSensor.h
* Support for Dust Sensor from WaveShare
*
*  Created on: Sep 13, 2020
*      Author: Denis Kudia
*/

#ifndef PI_LIBRARY_DustSensor_H_
#define PI_LIBRARY_DustSensor_H_

#include "item.h"
#include "AnalogDataReceiverItf.h"

namespace pirobot {
namespace dustsensor {

/**
 *  GPIO used for start stop dust sensore before and after measurement
 */

class DustSensor : public item::Item, public analogdata::AnalogDataReceiverItf{
public:
    DustSensor(
        const std::shared_ptr<pirobot::gpio::Gpio> gpio,
        const int pin,
        const std::string& name,
        const std::string& comment = std::string("Dust Sensor")
        )
        :item::Item(gpio, name, comment, item::ItemTypes::DustSensor), analogdata::AnalogDataReceiverItf(name, pin)

    {
    }


    virtual ~DustSensor() {

    }

    /**
     * Functiona called before and after measurement. Implement special functionality.
     */
    virtual bool before() override{
        return true;
    }

    virtual bool after() override{
        return true;
    }

    /*
    *
    */
    virtual const std::string printConfig() override{
        return analogdata::AnalogDataReceiverItf::printConfig();
    }


};

} //dustsensor
} //pirobot


#endif