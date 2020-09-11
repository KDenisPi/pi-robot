/*
* AnalogMeterSimple.h
* Support for Analog Meter that save inly last measured value
*
*  Created on: Sep 10, 2020
*      Author: Denis Kudia
*/

#ifndef PI_LIBRARY_AnalogMeterSimple_H_
#define PI_LIBRARY_AnalogMeterSimple_H_

#include <vector>

#include "item.h"
#include "AnalogDataReceiverItf.h"
#include "AnalogDataProviderItf.h"

namespace pirobot {
namespace analogmeter {

class AnalogMeterSimple : public item::Item{
public:
    AnalogMeterSimple(
        const std::shared_ptr<pirobot::analogdata::AnalogDataProviderItf> provider,
        const std::string& name,
        const std::vector<int>& pins,
        const std::string& comment = std::string("Analog Meter")
        ):
        item::Item(name, comment, item::ItemTypes::AnalogMeterSimple),
        m_provider(provider),
        _pin_count(0)
    {
        assert(provider);
        assert(pins.size()>0);


        if(m_provider){
            _pin_count = pins.size();
            for(int i = 0; i < _pin_count; i++){
                int pin = pins[i];
                if(_pins[i]){
                    continue; //duplicate pins
                }

                _pins[i] = std::make_shared<pirobot::analogdata::AnalogDataReceiverItf>("pin"+std::to_string(pin), pin);
                bool added = m_provider->register_data_receiver(pin, _pins[i]);
                if(added){
                    _pins[i]->activate();
                }
            }
        }
    }

    static const int Max_Analog_Inputs = 8;
    static const uint16_t Error_Value = 0xFFFF;


    /**
     *
     */
    virtual ~AnalogMeterSimple() {

    }

    /**
     *
     */
    const uint16_t get_data(const int pin){
        if(pin >= Max_Analog_Inputs || !_pins[pin]){
            return Error_Value;
        }

        return _pins[pin]->get_data();
    }

    /*
    *
    */
    virtual const std::string printConfig() override{
        std::string result = name();
        if(m_provider)
            result +=  " Provider: " + m_provider->pname();

        for(int i = 0; i < Max_Analog_Inputs; i++){
            if(_pins[i]){
                result = result + " Idx: " + std::to_string(_pins[i]->get_idx()) + " " + _pins[i]->pname();
            }
        }

        return result;
    }


private:
    std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf> _pins[Max_Analog_Inputs];
    int _pin_count;

    std::shared_ptr<pirobot::analogdata::AnalogDataProviderItf> m_provider;
};

}//analogmeter
}//pirobot

#endif