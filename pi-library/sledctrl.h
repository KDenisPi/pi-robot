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

#include "Threaded.h"
#include "CircularBuffer.h"

#include "SPI.h"
#include "item.h"
#include "sled.h"
#include "sled_data.h"

namespace pirobot {
namespace item {
namespace sledctrl {

using pled = std::shared_ptr<pirobot::item::SLed>;
using transf_type = std::pair<std::string, std::shared_ptr<pirobot::item::SledTransformer>>;

class SLedCtrl : public pirobot::item::Item, piutils::Threaded
{
public:
    SLedCtrl(const std::shared_ptr<pirobot::spi::SPI> spi,
        const int sleds,
        const std::string& name,
        const std::string& comment="",
        spi::SPI_CHANNELS channel = spi::SPI_CHANNELS::SPI_0) :
            item::Item(name, comment, item::ItemTypes::SLEDCRTL) ,
            _spi(spi), _spi_channel(channel), _data_buff(nullptr), _max_leds(0), _max_sleds(sleds), _data_buff_len(0) {

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " name " + name + " Max SLEDs: " + std::to_string(sleds));

        _transf = std::shared_ptr<piutils::circbuff::CircularBuffer<transf_type>>(new piutils::circbuff::CircularBuffer<transf_type>(20));
    }

    virtual ~SLedCtrl(){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " name " + name());

        piutils::Threaded::stop();

        while( !_transf->is_empty()){
            auto item = _transf->get();
            item.second->reset();
        }

        _sleds.empty();

        if( _data_buff != nullptr )
            free(_data_buff);
    }

    //Add LED Stripe
    void add_sled(const pled& led){
        if( _sleds.size() == _max_sleds){
            logger::log(logger::LLOG::ERROR, "LedCtrl", std::string(__func__) + " Too much SLEDs");
            return;
        }

        if( _max_leds < led->leds()){
            _max_leds = led->leds();
        }

        std::size_t bsize = get_buffer_length(led->leds(), led->stype());
        if( bsize > _data_buff_len )
            _data_buff_len = bsize;

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Added SLEDs " + led->name() +
            " Max LEDs now: " + std::to_string(_max_leds) + " Buffer size now: " + std::to_string(_data_buff_len));

        _sleds.push_back(led);
    }

    //Get LED stripes number
    const std::size_t sleds() const {
        return _sleds.size();
    }

    virtual void stop() override {
      logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started.");
      piutils::Threaded::stop();
    }

    /*
    * Allocate data buffer
    */
    virtual bool initialize() override {
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Allocate memory");
        prepare_bufeer();

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Start worker" );
        return piutils::Threaded::start<SLedCtrl>(this);
        //return true;
    }

    //
    //
    virtual const std::string printConfig() override {
        std::string result =  name() + " SPI Channel: " + std::to_string(_spi_channel) + " Maximun supported SLEDs: " +
            std::to_string(_max_sleds) + "\n";

        for (auto sled : _sleds){
            result += sled->printConfig();
        }
        return result;
    }

    /*
    *
    */
    void On() {
        _spi->set_channel_on( _spi_channel );
    }

    /*
    *
    */
    void Off() {
        _spi->set_channel_off( _spi_channel );
    }

    /*
    * Set some color for LEDs
    */
    void color(const int led, const uint32_t rgb, const std::size_t led_start, const std::size_t led_count) {
        if( led > sleds()){
            logger::log(logger::LLOG::ERROR, "SLED", std::string(__func__) + " Incorrect LEDS number");
            return;
        }

    }

    /*
    *
    */
    const transf_type& get_transformation(){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started" );
        return _transf->get();
    }

    /*
    *
    */
    void add_transformation(const transf_type& transf){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started" );

        _transf->put(std::move(transf));
        cv.notify_one();
    }

    /*
    *
    */
    void refresh(){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started" );

        this->On();

        for (auto sled  : _sleds )
        {
      	    int lcount = sled->leds();
            const pirobot::item::SLedType stp = sled->stype();
            const std::size_t blen = get_buffer_length(lcount, stp);

            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Procedd LED stripe with : " +
                    std::to_string(lcount) + " Type: " + std::to_string(stp));

            const std::uint8_t* lgm = sled->gamma();
            const std::uint32_t* ldata = sled->leds_data();

            const size_t  dlen = get_data_length();
            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Initialize buffer Len: " + std::to_string(dlen));
            std::memset( (void*)_data_buff, 0, dlen);

            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Fill data buffer" );
            for( std::size_t lidx = 0; lidx < lcount; lidx++ ){

                // Convert 0RGB to R,G,B
                std::uint8_t rgb[3] = {
                    lgm[ (ldata[lidx] & 0xFF) ],
                    lgm[ ((ldata[lidx] >> 8 ) & 0xFF) ],
                    lgm[ ((ldata[lidx] >> 16 ) & 0xFF) ]
                };

                if( stp == pirobot::item::SLedType::WS2812B){
                    //
                    // Replace each R,G,B byte by 3 bytes
                    //
                    for(int rgb_idx = 0; rgb_idx < 3; rgb_idx++){
                        int idx = (lidx + rgb_idx)*3;
                        _data_buff[idx] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]];
                        _data_buff[idx+1] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]+1];
                        _data_buff[idx+2] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]+2];
                    }
                }
                else{
                    int idx = lidx*3;
                    _data_buff[idx + 0] |= rgb[0];
                    _data_buff[idx + 1] |= rgb[1];
                    _data_buff[idx + 2] |= rgb[2];
                }
            }

            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Write to SPI: " +  std::to_string(blen));

            //Write data to SPI
            _spi->data_rw(_data_buff, blen);
            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Writed to SPI: " +  std::to_string(blen));
        }

        this->Off();
        // Wait 500 ms before sending new data
        std::this_thread::sleep_for(std::chrono::microseconds(500));

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Finished" );
    }

    /*
    * Apply transformation
    */
    const bool aplply_transformation(transf_type& transf){
        std::string led_idx = transf.first;
        bool applied = false;

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Transformation : " + typeid(transf.second.get()).name());

        for (auto sled  : _sleds )
        {
            /*
            * Check should we apply this transformation for this LED stripe
            */
      	    if(!led_idx.empty() && led_idx != sled->name() ){
                  continue;
            }

            transf.second->transform( sled->leds_data(), sled->leds());
            applied = true;
        }

        if(applied){

            /*
            * Refresh LEDs
            */
            refresh();

            transf.second->delay_after();
        }

        return transf.second->is_finished();
    }

    /*
    *
    */
    const bool is_transformation() const {
        return !_transf->is_empty();
    }


    /*
    *
    *
    */
    static void worker(SLedCtrl* p){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started" );

        auto fn = [p]{return (p->is_stop_signal() || p->is_transformation());};

        while(!p->is_stop_signal()){
            //wait until stop signal will be received or we will have steps for processing
            {
                std::unique_lock<std::mutex> lk(p->cv_m);
                p->cv.wait(lk, fn);
            }

            while( !p->is_stop_signal() && p->is_transformation() ){
                logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Transformation detected" );

                auto transf = p->get_transformation();
                bool last_attempt = p->aplply_transformation(transf);

                while(!p->is_stop_signal() && !last_attempt){
                    last_attempt = p->aplply_transformation(transf);

                    logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Transformation. Last attempt: " + std::to_string(last_attempt) );
                }

            }
        }

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Finished" );
    }


private:
    std::vector<pled> _sleds;   //LED stripe list
    int _max_leds;              //Maximum number of LEDs
    int _max_sleds;             //maximun number of LED Stripes supported by controller

    std::shared_ptr<pirobot::spi::SPI> _spi;
    int _spi_channel;


    std::uint8_t* _data_buff;   //Data buffer for SPI
    std::size_t _data_buff_len;

    std::shared_ptr<piutils::circbuff::CircularBuffer<transf_type>> _transf;
    /*
    *
    */
    const std::size_t get_data_length() const {
        return _data_buff_len;
    }

    void prepare_bufeer() {
        if( _data_buff == nullptr ){
            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Create data buffer: " + std::to_string(_data_buff_len) );
            _data_buff = (uint8_t*) std::malloc(sizeof(uint8_t)*_data_buff_len);
        }
    }

    /*
    *   Calculate buffer length
    *
    *   LEDs * 3(RGB) * 3(3 SPI bits for 1 data bit) + 15 (50us)
    */
    const std::size_t get_buffer_length(const int leds, pirobot::item::SLedType stype) {
        std::size_t bsize =  leds * 3;
        if( stype == pirobot::item::SLedType::WS2812B)
            bsize = bsize * 3 + 15;
        return bsize;
    }
};

}//sledctrl
}//namespace item
}//namespace pirobot

#endif
