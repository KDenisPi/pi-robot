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
#include <cstdlib>
#include <vector>
#include <memory>

#include "item.h"

namespace pirobot {
namespace item {

enum SLedType {
    WS2810 = 0,
    WS2812B
};

class SLed: public Item {
public:

    /*
    *
    */
    SLed(const int led_num, const SLedType stype, const std::string& name, const std::string& comment="") :
        Item( name, comment, ItemTypes::SLED), _leds(led_num), _stype(stype) {
        assert( _leds > 0 );

        logger::log(logger::LLOG::DEBUG, "SLED", std::string(__func__) + " name " + name + " LEDs: " + std::to_string(_leds) + " Len: " + std::to_string(sizeof(uint32_t)*_leds));

        _leds_data = std::unique_ptr<uint32_t[]>(new uint32_t[_leds]);
        for(int i = 0; i < _leds; i++){
            _leds_data[i] = (uint32_t)0;
        }

        const size_t gsize = 256;
        _gamma = std::unique_ptr<uint8_t[]>(new uint8_t[gsize]);
        for(int i = 0; i < gsize; i++){
            _gamma[i] = (uint8_t)i;
        }
    }

    virtual const std::string printConfig() override {
        std::string result =  name() + " LEDs:  " +  std::to_string(_leds) + " Type: " + (_stype == SLedType::WS2810 ? "WS2810" : "WS2812B") + "\n";
        return result;
    }

    /*
    *
    */
    virtual ~SLed() {
        logger::log(logger::LLOG::DEBUG, "SLED", std::string(__func__) + " name " + name());

    }

    // Get LEDs number
    const std::size_t leds() const {
        return _leds;
    }

    std::uint32_t* leds_data() const {
        return _leds_data.get();
    }

    const std::uint8_t* gamma() const {
        return _gamma.get();
    }

    const SLedType stype() const {
        return _stype;
    }

private:
    std::size_t _leds;          //LEDs number
    SLedType _stype;            //SLed Type
    std::unique_ptr<uint32_t[]> _leds_data;       //array of LEDs data
    std::unique_ptr<uint8_t[]>  _gamma;           //LED gamma
};


}//namespace item
}//namespace pirobot

#endif
