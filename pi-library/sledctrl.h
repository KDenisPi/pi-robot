/*
 * sledctrl.h
 *
 * LED Stripe Controller
 *
 *  Created on: Dec 14, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SLED_CTRL_H_
#define PI_LIBRARY_SLED_CTRL_H_

#include <cstring>
#include <memory>
#include <vector>

#include "SPI.h"
#include "item.h"
#include "sled.h"
#include "sled_data.h"

namespace pirobot {
namespace item {
namespace sledctrl {

using pled = std::shared_ptr<pirobot::item::SLed>;

class SLedCtrl : public pirobot::item::Item
{
public:
    SLedCtrl(const std::shared_ptr<pirobot::spi::SPI> spi,
        const int sleds,
        const std::string& name,
        const std::string& comment="",
        spi::SPI_CHANNELS channel = spi::SPI_CHANNELS::SPI_0) :
            item::Item(name, comment, item::ItemTypes::SLEDCRTL) ,
            _spi(spi), _spi_channel(channel), _data_buff(nullptr), _max_leds(0), _max_sleds(sleds) {

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " name " + name + " Max SLEDs: " + std::to_string(sleds));
    }

    virtual ~SLedCtrl(){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " name " + name());

        _sleds.empty();

        if( _data_buff != nullptr )
            delete[] _data_buff;
    }

    //Add LED Stripe
    void add_sled(const pled&& led){
        if( _sleds.size() == _max_sleds){
            logger::log(logger::LLOG::ERROR, "LedCtrl", std::string(__func__) + " Too much SLEDs");
            return;
        }

        if( _max_leds < led->leds()){
            _max_leds = led->leds();
        }

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Added SLEDs " + led->name() + " Max LEDs now: " + std::to_string(_max_leds));

        _sleds.push_back(led);
    }

    //Get LED stripes number
    const std::size_t sleds() const {
        return _sleds.size();
    }

    virtual const std::string printConfig() override {
        std::string result =  name() + " SPI Channel: " + std::to_string(_spi_channel) + " Maximun supported SLEDs: " +
            std::to_string(_max_sleds) + "\n";
        for (auto sled = _sleds.cbegin(); sled != _sleds.cend(); sled++){
            result += sled->get()->printConfig();
        }
        return result;
    }

    /*
    * Set some color for LEDs
    */
    void set_color(const int led, const uint32_t rgb, const std::size_t led_start = 0, const std::size_t led_end = 0) {
        if( led > sleds()){
            logger::log(logger::LLOG::ERROR, "SLED", std::string(__func__) + " Incorrect LEDS number");
            return;
        }

        _sleds[led]->set_color(rgb, led_start, led_end);
    }

    //
    void refresh(){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started" );

        if( _data_buff == nullptr ){
            _data_buff = new std::uint8_t( calculate_buffer() );
        }

        //
        _spi->set_channel_on( _spi_channel );

        for (auto sled = _sleds.cbegin(); sled != _sleds.cend(); sled++){
            const std::uint8_t* gm = sled->get()->gamma();
            const std::uint32_t* data = sled->get()->data();

            memset( _data_buff, 0, sizeof(_data_buff));

            for( std::size_t lidx = 0; lidx < sled->get()->leds(); lidx++ ){
                // Convert 0RGB to R,G,B
                std::uint8_t rgb[3] = { gm[ (data[lidx] & 0xFF) ],
                    gm[ ((data[lidx] >> 8 ) & 0xFF) ],
                    gm[ ((data[lidx] >> 16 ) & 0xFF) ]
                };

                //
                // Write 3 bytes for each R,G,B
                //
                for(int rgb_idx = 0; rgb_idx < 3; rgb_idx++){
                    int idx = (lidx + rgb_idx)*3;
                    _data_buff[idx] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]];
                    _data_buff[idx+1] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]+1];
                    _data_buff[idx+2] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]+2];
                }
            }

            //Write data to SPI
            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Write to SPI: " +  std::to_string(sizeof(_data_buff)));

            _spi->data_rw( _data_buff, sizeof(_data_buff));

        }

        //
        _spi->set_channel_off( _spi_channel );

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Finished" );
    }

private:
    std::vector<pled> _sleds;   //LED stripe list
    int _max_leds;              //Maximum number of LEDs
    int _max_sleds;             //maximun number of LED Stripes supported by controller

    std::shared_ptr<pirobot::spi::SPI> _spi;
    int _spi_channel;


    std::uint8_t* _data_buff;   //Data buffer for SPI

    /*
    *   Calculate buffer length
    *
    *   LEDs * 3(RGB) * 3(3 SPI bits for 1 data bit) + 15 (50us)
    */
    std::size_t calculate_buffer() {
        return _max_leds * 3 * 3 + 15;
    }
};

}//sledctrl
}//namespace item
}//namespace pirobot

#endif
