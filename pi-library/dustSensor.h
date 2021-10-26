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

/**
--------------------------------------------------------------------------------------------
|PM2.5 density value (μg/m3) |Air quality index | Air quality level | Air quality evaluation
--------------------------------------------------------------------------------------------
|0-35  			     | 0-50             | Level 1           | Excellent
|35-75                       | 51-100           | Level 2           | Average
|75-115                      | 101-150          | Level 3           | Light pollution
|115-150                     | 151-200          | Level 4           | Moderate pollution
|150-250                     | 201-300          | Level 5           | Heavy pollution
|250-500                     | >300             | Level 6           | Serious pollution
*/


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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //280 by documentation
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
        float voltage = ((SysVoltage * f_raw_data) / 1024.0) * 1000;
        float density = 0.0;

        /*
        voltage to density
        */
        if(voltage >= NoDustVoltage)
        {
            density = (voltage - NoDustVoltage) * ConversionRatio;
        }

        logger::log(logger::LLOG::INFO, "DustS", std::string(__func__) + " Raw: " + std::to_string(raw_data) + " Filtered: " + std::to_string(f_raw_data) + " Voltage: " + std::to_string(voltage) + " Density: " + std::to_string(density));

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

const float SysVoltage       = 3.3; //3,3V
const float NoDustVoltage    = 400;  //400 mV = 0.4V
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
    if(m == 0)
      return _sum / 10.0;


    _sum -= _buff[0];
    for(int i = 0; i < 9; i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    _sum += _buff[9];

    result = _sum / 10.0;
  }

/*
  {
    char sbuff[128];
    std::sprintf(sbuff, "Filter [%d] %d %d %d %d %d %d %d %d %d %d", result, _buff[0], _buff[1], _buff[2], _buff[3], _buff[4], _buff[5], _buff[6], _buff[7],  _buff[8], _buff[9]);
    logger::log(logger::LLOG::DEBUG, "DustS", std::string(__func__) + std::string(sbuff));
  }
*/

  return result;
}


};

} //dustsensor
} //item
} //pirobot


#endif
