/*
* dustSensor.h
* Support for Dust Sensor from WaveShare
*
*  Created on: Sep 13, 2020
*      Author: Denis Kudia
*/

#ifndef PI_LIBRARY_DustSensor_H_
#define PI_LIBRARY_DustSensor_H_

#include <cmath>

#include "logger.h"
#include "item.h"
#include "AnalogDataReceiverItf.h"

namespace pirobot {

namespace item {
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
        logger::log(logger::LLOG::DEBUG, "DustS", std::string(__func__));
        get_gpio()->Low();
    }


    virtual ~DustSensor() {
        logger::log(logger::LLOG::DEBUG, "DustS", std::string(__func__));
        get_gpio()->Low();
    }

    /**
     * Functiona called before and after measurement. Implement special functionality.
     */
    virtual bool before() override{
        logger::log(logger::LLOG::DEBUG, "DustS", std::string(__func__));
        get_gpio()->High();
        std::this_thread::sleep_for(std::chrono::milliseconds(280));
        return true;
    }

    virtual bool after() override{
        logger::log(logger::LLOG::DEBUG, "DustS", std::string(__func__));
        get_gpio()->Low();
        return true;
    }


    virtual const uint16_t get_data() override{
        logger::log(logger::LLOG::DEBUG, "DustS", std::string(__func__));

        int raw_data = analogdata::AnalogDataReceiverItf::get_data();
        int f_raw_data = filter(raw_data);

        /*
        covert voltage (mv)
        */
        float voltage = (SysVoltage / 1024.0) * f_raw_data * 11;
        float density = 0.0;

        /*
        voltage to density
        */
        if(voltage >= NoDustVoltage)
        {
            voltage -= NoDustVoltage;
            density = voltage * ConversionRatio;
        }

        logger::log(logger::LLOG::DEBUG, "DustS", std::string(__func__) + " Raw: " + std::to_string(raw_data) + " Filtered: " + std::to_string(f_raw_data) +
                " Voltage: " + std::to_string(voltage) + " Density: " + std::to_string(density));

        return (uint16_t)(round(density));
    }

    /*
    *
    */
    virtual const std::string printConfig() override{
        return analogdata::AnalogDataReceiverItf::printConfig();
    }

private:

bool _first_value = true;
int _buff[10];
int _sum = 0;

const float SysVoltage       = 3500; //3,5V
const float NoDustVoltage    = 400;  //0,4V
const float ConversionRatio  = 0.2;  //ug/mmm / mv

/**
 *
 */
int filter(int m)
{
  int result;

  if(_first_value)
  {
    // Do not fill array using zero value
    if(m == 0)
      return m;

    _first_value = false;
    for(int i = 0; i < 10; i++)
    {
      _buff[i] = m;
      _sum += _buff[i];
    }
    result = m;
  }
  else
  {
    _sum -= _buff[0];
    for(int i = 0; i < 9; i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    _sum += _buff[9];

    result = _sum / 10.0;
  }

  {
    char sbuff[128];
    std::sprintf(sbuff, "Filter [%d] %d %d %d %d %d %d %d %d %d %d", result, _buff[0], _buff[1], _buff[2], _buff[3], _buff[4], _buff[5], _buff[6], _buff[7],  _buff[8], _buff[9]);
    logger::log(logger::LLOG::DEBUG, "DustS", std::string(__func__) + std::string(sbuff));
  }

  return result;
}


};

} //dustsensor
} //item
} //pirobot


#endif