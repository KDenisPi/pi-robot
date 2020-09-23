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
        const std::string& comment = std::string("Analog Meter")
        ):
        item::Item(name, comment, item::ItemTypes::AnalogMeterSimple),
        m_provider(provider)
    {
        assert(provider);
        logger::log(logger::LLOG::DEBUG, "AnalgMSm", std::string(__func__) + " " +  item::Item::name() + " Provider: " + m_provider->pname());
    }

    static const int Max_Analog_Inputs = 8;
    static const uint16_t Error_Value = 0xFFFF;

    /**
     *
     */
    virtual ~AnalogMeterSimple() {
        logger::log(logger::LLOG::DEBUG, "AnalgMSm", std::string(__func__) + " " +  name());
    }

    /**
     * Add receivers
     */
    bool add_receiver(const int pin, const std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf>& receiver){
        logger::log(logger::LLOG::DEBUG, "AnalgMSm", std::string(__func__) + " Pin: " +  std::to_string(pin));

        bool result = false;
        if(m_provider){
            if(_pins[pin]){
                return false; //duplicate pin
            }

            _pins[pin] = receiver;
            //std::make_shared<pirobot::analogdata::AnalogDataReceiverItf>("pin"+std::to_string(pin), pin);
            bool added = m_provider->register_data_receiver(pin, _pins[pin]);
            if(added){
                _pins[pin]->activate();
                result = true;
            }
        }

        logger::log(logger::LLOG::DEBUG, "AnalgMSm", std::string(__func__) + " Pin: " +  std::to_string(pin) + " Result: " + std::to_string(result));
        return result;
    }

    /**
     *
     */
    const uint16_t get_data(const int pin){
        uint16_t result = Error_Value;
        if(pin >= Max_Analog_Inputs || !_pins[pin]){
            logger::log(logger::LLOG::ERROR, "AnalgMSm", std::string(__func__) + " Invalid pin: " + std::to_string(pin) );
            return result;
        }

        result = _pins[pin]->get_data();
        return result;
    }

    /**
     *
     */
    const float get_value(const int pin){
        float result = 9999.99;
        if(pin >= Max_Analog_Inputs || !_pins[pin]){
            logger::log(logger::LLOG::ERROR, "AnalgMSm", std::string(__func__) + " Invalid pin: " + std::to_string(pin) );
            return result;
        }

        result = _pins[pin]->get_value();
        return result;
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
    std::shared_ptr<pirobot::analogdata::AnalogDataProviderItf> m_provider;
};

}//analogmeter
}//pirobot

#endif