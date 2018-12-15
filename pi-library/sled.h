/*
 * sled.h
 *
 * LED Stripe
 *
 *  Created on: Dec 11, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SLED_H_
#define PI_LIBRARY_SLED_H_

#include <cstring>
#include "item.h"

namespace pirobot {
namespace item {

class SLed: public Item {
public:

    /*
    *
    */
    SLed(const int led_num, const std::string& name, const std::string& comment="") :
        Item( name, comment, ItemTypes::SLED), _leds(led_num){
        assert( _leds > 0 );

        _leds_data = new uint32_t[_leds];
        std::memset(_leds_data, 0, sizeof(uint32_t)*_leds);

        _gamma = new uint8_t[256];
        for(int i = 0; i < 256; i++){
            _gamma[i] = i;
        }
    }

    virtual const std::string printConfig() override {
        std::string result =  name() + " LEDs:  " +  std::to_string(_leds) + "\n";
        return result;
    }

    /*
    *
    */
    virtual ~SLed() {
        delete[] _leds_data;
        delete[] _gamma;
    }

    // Get LEDs number
    const std::size_t leds() const {
        return _leds;
    }

    const std::uint32_t* data() {
        return _leds_data;
    }

    const std::uint8_t* gamma() {
        return _gamma;
    }

private:
    std::size_t _leds;                  //LEDs number
    uint32_t* _leds_data;       //array of LEDs data
    uint8_t*  _gamma;           //LED gamma
};

}//namespace item
}//namespace pirobot

#endif
