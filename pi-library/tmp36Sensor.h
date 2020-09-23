/*
* tmp36Sensor.h
* Support for temperature sensor Tmp36
*
*  Created on: Sep 22, 2020
*      Author: Denis Kudia
*/

#ifndef PI_LIBRARY_Tmp36Sensor_H_
#define PI_LIBRARY_Tmp36Sensor_H_

#include <cmath>

#include "logger.h"
#include "item.h"
#include "AnalogDataReceiverItf.h"

namespace pirobot {

namespace item {
namespace tmp36sensor {

/**
 *
 */

class Tmp36Sensor : public item::Item, public analogdata::AnalogDataReceiverItf{
public:
    Tmp36Sensor(
        const int pin,
        const std::string& name,
        const std::string& comment = std::string("Tmp36 Sensor")
        )
        :item::Item(name, comment, item::ItemTypes::Tmp36), analogdata::AnalogDataReceiverItf(name, pin)

    {
        logger::log(logger::LLOG::DEBUG, "Tmp36", std::string(__func__));
    }


    virtual ~Tmp36Sensor() {
        logger::log(logger::LLOG::DEBUG, "Tmp36", std::string(__func__));
    }

    virtual const float get_value() override{
        logger::log(logger::LLOG::DEBUG, "Tmp36", std::string(__func__));

        int raw_data = get_data();
        float voltage = ((SysVoltage * raw_data) / 1024.0) * 1000; //mv
        float temp = (voltage-500.0)/10.0;

        logger::log(logger::LLOG::INFO, "Tmp36", std::string(__func__) + " Raw: " + std::to_string(raw_data) + " Voltage: " + std::to_string(voltage) + " Temp (C): " + std::to_string(temp));
        return temp;
    }

    /*
    *
    */
    virtual const std::string printConfig() override{
        return analogdata::AnalogDataReceiverItf::printConfig();
    }

private:

const float SysVoltage       = 3.3; //3,3V

};

} //tmp36sensor
} //item
} //pirobot


#endif
