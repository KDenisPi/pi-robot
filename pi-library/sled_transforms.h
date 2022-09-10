/*
 * sled_transforms.h
 *
 * LED Stripe Transformations
 *
 *  Created on: Dec 11, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SLED_TRANSFORMS_H_
#define PI_LIBRARY_SLED_TRANSFORMS_H_

#include "sled.h"

namespace pirobot {
namespace sled {

struct PutInterval {uint16_t sec; uint16_t ms; };
using SLedRep = struct PutInterval;

/*
* Base class for LED stripe transformation
*/
class SledTransformer {
public:
    SledTransformer(const int cnt = 1, const SLedRep& delay_bw = {0, 500}, const bool llast = false) :
        _count(cnt), _org_count(cnt), _delay_bw(delay_bw), _llast(llast) {}
    virtual ~SledTransformer() {}

    virtual bool transform(uint32_t* ldata = nullptr, const std::size_t llen = 0) {return attempt();}

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

    /*
    *
    */
    const bool is_finished() {
        return (_count == 0);
    }

    /*
    * Is the last transformation in the list
    */
    const bool is_last() const{
        return _llast;
    }
    /*
    * Make some delay after transformation applying
    */
    virtual void delay_after(){
        if( _delay_bw.sec > 0 ){
            std::this_thread::sleep_for(std::chrono::seconds(_delay_bw.sec));
        }
        else if ( _delay_bw.ms > 0 )
            std::this_thread::sleep_for(std::chrono::milliseconds(_delay_bw.ms));
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
    bool _llast;

    SLedRep _delay_bw;


    /*
    * Name of LED stripe this transformation should be applied.
    * Empty means apply for all LED stripes
    */
    std::string _sled_idx;
};

/*
* Shift direction
*/
enum Direction {
    ToLeft,
    ToRight
};

/*
* NOP. Last transformation in the list
*/
class NopTransformation : public SledTransformer {
public:
    NopTransformation(const std::string& evt_name, std::function<void(const std::string&)> ntf_finished) :
        SledTransformer(1, {0, 500}, true), _event(evt_name), _ntf_finished(ntf_finished) {}
    virtual ~NopTransformation() {}

    virtual bool transform(uint32_t* ldata, const std::size_t llen) override {
        /*
        * Send notification
        */
        if( _ntf_finished ){
            _ntf_finished(_event);
        }

        return SledTransformer::attempt();
    }

    std::function<void(const std::string&)> _ntf_finished;
private:
    std::string _event;
};

/*
* Switch all LEDs OFF
*/
class OffTransformation : public SledTransformer {
public:
    OffTransformation() : SledTransformer() {}
    virtual ~OffTransformation() {}

    virtual bool transform(uint32_t* ldata, const std::size_t llen) override {
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
    ShiftTransformation(const int cnt, Direction drct = Direction::ToRight,
            const SLedRep& delay_bw = {0, 250}, const int swpos = 1)
        : SledTransformer(cnt, delay_bw), _dict(drct), _switch_pos(swpos) {

    }
    virtual ~ShiftTransformation() {}


    virtual bool transform(uint32_t* ldata, const std::size_t llen) override{

        if( is_finished() ){
            return true;
        }

        std::size_t nsw = ((_switch_pos > 0 && _switch_pos < llen) ? _switch_pos : llen-1);

        uint32_t led_mv;
        int j;
        for(int i = 0 ; i < nsw; i++){
            if(_dict == Direction::ToRight){
                led_mv = ldata[llen -1];
                for(j = llen-2; j >= 0; j--){
                    ldata[j+1] = ldata[j];
                }
                ldata[0] = led_mv;
            }
            else{
                led_mv = ldata[0];
                for(j = 1; j < llen; j++){
                    ldata[j-1] = ldata[j];
                }
                ldata[llen-1] = led_mv;
            }
        }

        return SledTransformer::attempt();
    }

private:
    Direction _dict;
    int _switch_pos;
};

/*
* Set group of color for one or multiple LEDs
*/
class SetColorGroupTransformation : public SledTransformer {
public:
    SetColorGroupTransformation(std::vector<uint32_t>& rgbs, const std::size_t led_start = 0, const std::size_t repeat = -1)
        : SledTransformer(), _led_start(led_start), _repeat(repeat) {

        _rgbs.swap( rgbs );
    }

    virtual ~SetColorGroupTransformation() {}

    virtual bool transform(uint32_t* ldata, const std::size_t llen) override{
        std::size_t nset = ( (_repeat > 0 && (_led_start + _rgbs.size()*_repeat) < llen) ? _rgbs.size()*_repeat : llen);
        if( nset > 32 ){
            nset = 32;
        }

        logger::log(logger::LLOG::DEBUG, "SLED", std::string(__func__) + " Start: " + std::to_string(_led_start) + " Last: " + std::to_string(nset) + " RGBs: " + std::to_string(_rgbs.size()));

        int j = 0;
        for( int i = 0; i < nset; i++){
            ldata[_led_start + i] = _rgbs[j++];
            if(j == _rgbs.size()) j = 0;
        }

        return SledTransformer::attempt();
    }

private:
    std::vector<uint32_t> _rgbs;
    std::size_t _led_start;
    std::size_t _repeat;
};

/*
* Set some color for one or multiple LEDs
*/
class SetColorTransformation : public SledTransformer {
public:
    SetColorTransformation(const uint32_t rgb, const std::size_t led_start, const std::size_t led_count) :
        SledTransformer(), _rgb(rgb), _led_start(led_start), _led_count(led_count) {}
    virtual ~SetColorTransformation() {}

    virtual bool transform(uint32_t* ldata, const std::size_t llen) override{
        std::size_t nset = ((_led_start + _led_count) >= llen ? llen - _led_start : _led_count);
        if(nset < 0 || _led_count == 0){
            logger::log(logger::LLOG::ERROR, "SLED", std::string(__func__) + " Incorrect LED index");
            return false;
        }
        else {
            for( int i = 0; i < nset; i++){
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


} //namespace sled
} //namespace pirobot 

#endif