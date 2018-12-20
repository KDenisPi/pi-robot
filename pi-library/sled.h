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

        _leds_data = (uint32_t*)std::malloc(sizeof(uint32_t) * _leds);
        std::memset((void*)_leds_data, 0, sizeof(uint32_t)*_leds);

        _gamma = (uint8_t*)std::malloc(sizeof(uint8_t)*256);
        for(int i = 0; i < 256; i++){
            _gamma[i] = i;
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

        std::free(_leds_data);
        std::free(_gamma);
    }

    // Get LEDs number
    const std::size_t leds() const {
        return _leds;
    }

    std::uint32_t* leds_data() {
        return _leds_data;
    }

    const std::uint8_t* gamma() {
        return _gamma;
    }

    const SLedType stype() const {
        return _stype;
    }

private:
    std::size_t _leds;          //LEDs number
    SLedType _stype;            //SLed Type
    uint32_t* _leds_data;       //array of LEDs data
    uint8_t*  _gamma;           //LED gamma
};

/*
* Base class for LED stripe transformation
*/
class SledTransformer {
public:
    SledTransformer(const int cnt = 1) : _count(cnt), _org_count(cnt) {}
    virtual ~SledTransformer() {}

    virtual bool transform(uint32_t* ldata, const std::size_t llen){}

    /*
    *  Decrease attempt counter
    *  Return TRUE - if there was the last attempt, otherwise need execute transformation more time
    */
    const bool attempt()
    {
        if( !is_finished() && _count > 0)
            _count--;

        logger::log(logger::LLOG::DEBUG, "SLED", std::string(__func__) + " Count: " + std::to_string(_count));
        return is_finished();
    }

    const bool is_finished() {
        return (_count == 0);
    }

    /*
    * Make some delay after transformation applying
    */
    virtual void delay_after(){
    }

    /*
    * Restore transformation counter
    */
    virtual void reset() {
        _count = _org_count;
    }


    const std::string get_sled_idx() const {
        return _sled_idx;
    }

    void set_sled_index(const std::string& sled_idx){
        _sled_idx = sled_idx;
    }

protected:
    int _count; //number of transformations, stop when equal 0, -1 - infinitely
    int _org_count;

    /*
    * Name of LED stripe this transformation should be applied.
    * Empty means apply for all LED stripes
    */
    std::string _sled_idx;
};

enum Direction {
    ToLeft,
    ToRight
};

/*
* Switch all LEDs OFF
*/
class OffTransformation : public SledTransformer {
public:
    OffTransformation() : SledTransformer() {}
    virtual ~OffTransformation() {}

    virtual bool transform(uint32_t* ldata, const std::size_t llen) override{
        for( int i = 0; i < llen; i++){
            ldata[i] = 0;
        }
        return SledTransformer::attempt();
    }
};

/*
* ShiftTransformation
*
* Shift LEDs to right/left for some number positions
*/
class ShiftTransformation : public SledTransformer {
public:
    ShiftTransformation(const int cnt, const Direction drct = Direction::ToRight, const int swpos = 1)
        : SledTransformer(cnt), _dict(drct), _switch_pos(swpos) {

    }
    virtual ~ShiftTransformation() {}


    virtual bool transform(uint32_t* ldata, const std::size_t llen) override{
        if( is_finished() )
            return true;

        uint32_t led_mv;
        int j;
        for(int i = 0 ; i < _switch_pos; i++){
            if(_dict == Direction::ToRight){
                led_mv = ldata[llen -1];
                for(j = llen-2; j >= 0; j--) ldata[j+1] = ldata[j];
                ldata[0] = led_mv;
            }
            else{
                led_mv = ldata[0];
                for(j = 1; j < llen; j++) ldata[j-1] = ldata[j];
                ldata[llen-1] = led_mv;
            }
        }

        return SledTransformer::attempt();
    }

    /*
    * Make some delay after transformation applying
    */
    virtual void delay_after() override {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

private:
    Direction _dict;
    int _switch_pos;
};

class SetColorTransformation : public SledTransformer {
public:
    SetColorTransformation(const uint32_t rgb, const std::size_t led_start, const std::size_t led_count) :
        SledTransformer(1), _rgb(rgb), _led_start(led_start), _led_count(led_count) {}
    virtual ~SetColorTransformation() {}

    virtual bool transform(uint32_t* ldata, const std::size_t llen) override{
        if( (_led_start + _led_count) >= llen){
            logger::log(logger::LLOG::ERROR, "SLED", std::string(__func__) + " Incorrect LED index");
        }
        else {
            for( int i = 0; i < _led_count; i++){
                ldata[_led_start + i] = _rgb;
            }
        }

        return SledTransformer::attempt();
    }

private:
    uint32_t _rgb;
    std::size_t _led_start;
    std::size_t _led_count;
};

}//namespace item
}//namespace pirobot

#endif
