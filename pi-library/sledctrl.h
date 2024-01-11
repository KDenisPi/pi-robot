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

#include "item.h"
#include "sled.h"
#include "sled_transforms.h"
#include "sled_data.h"

namespace pirobot {
namespace item {
namespace sledctrl {

using pled = std::shared_ptr<pirobot::item::SLed>;
using transf_type = std::pair<std::string, std::shared_ptr<pirobot::sled::SledTransformer>>;

class SLedCtrl : public pirobot::item::Item, public piutils::Threaded
{
public:
    SLedCtrl(item::ItemTypes itype,
        const int sleds,	//Maximum supported number of Stripe LEDs (no LEDs)
        const std::string& name,
        const std::string& comment="") :
            item::Item(name, comment, itype) ,
             _data_buff(nullptr), _max_leds(0), _max_sleds(sleds), _data_buff_len(0), _transf_idx(0) {

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " name " + name + " Max SLEDs: " + std::to_string(sleds));
    }

    SLedCtrl(item::ItemTypes itype,
        const int sleds,
        const std::string& name,
        const std::shared_ptr<pirobot::gpio::Gpio> gpio,
        const std::string& comment="") :
            item::Item(gpio, name, comment, itype) ,
             _data_buff(nullptr), _max_leds(0), _max_sleds(sleds), _data_buff_len(0), _transf_idx(0) {

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " name " + name + " Max SLEDs: " + std::to_string(sleds));
    }

    virtual ~SLedCtrl(){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " name " + name());

        piutils::Threaded::stop();

        for( auto item : _transf ){
            item.second->reset();
        }

        _sleds.clear();

        if( _data_buff != nullptr )
            free(_data_buff);
    }

    virtual bool write_data(uint8_t* data, int len) = 0;
    virtual void On() = 0;
    virtual void Off() = 0;

    /*
    * Add Right shift transformation
    */
    void add_r_shift(const int shift_count){
        this->transformation_add( std::make_pair(std::string("SHIFT_R"), std::shared_ptr<pirobot::sled::SledTransformer>(new pirobot::sled::ShiftTransformation(shift_count, pirobot::sled::Direction::ToRight))));
    }

    /*
    * Add Left shift transformation
    */
    void add_l_shift(const int shift_count){
        this->transformation_add( std::make_pair(std::string("SHIFT_R"), std::shared_ptr<pirobot::sled::SledTransformer>(new pirobot::sled::ShiftTransformation(shift_count, pirobot::sled::Direction::ToLeft))));
    }

    /*
    * Add LED Off tranformation
    */
    void add_led_off(const bool clear_before = false){
        if(clear_before){
            transformations_clear();
        }

        this->transformation_add( std::make_pair(std::string("LED_OFF"), std::shared_ptr<pirobot::sled::SledTransformer>(new pirobot::sled::OffTransformation())));
    }

    /*
    * Add SET color for group of LEDs transformation
    */
   void add_copy_rgbs(std::vector<uint32_t>& rgbs, const int stpos = 0, const int repeat = -1){
        this->transformation_add( std::make_pair(std::string("SET_RGBS"), std::shared_ptr<pirobot::sled::SledTransformer>(new pirobot::sled::SetColorGroupTransformation(rgbs, stpos, repeat))));
   }

    /*
    * Add SET color for group of LEDs transformation
    */
   void add_copy_rgb(const std::uint32_t rgb, const int stpos = 0, const int leds = 1){
        this->transformation_add( std::make_pair(std::string("SET_RGB"), std::shared_ptr<pirobot::sled::SledTransformer>(new pirobot::sled::SetColorTransformation(rgb, stpos, leds))));
   }

    /*
    * Add LAST transformation reaction
    */
   void add_last_operation(const std::string& evt, std::function<void(const std::string&)> ntf_finished){
        this->transformation_add( std::make_pair(std::string("LAST_OP"), std::shared_ptr<pirobot::sled::SledTransformer>(new pirobot::sled::NopTransformation(evt,ntf_finished))));
   }

    /**
     * @brief Add LED stripe to the controller
     *
     * @param led - SLed Item
     */
    bool add_sled(const pled& led){

        if( _sleds.size() >= _max_sleds){
            logger::log(logger::LLOG::ERROR, "LedCtrl", std::string(__func__) + " Too much SLEDs");
            return false;
        }

        if( _max_leds < led->leds()){
            _max_leds = led->leds();
        }

        std::size_t bsize = led->get_buffer_length();
        if( bsize > _data_buff_len )
            _data_buff_len = bsize;

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Added SLEDs " + led->name() + " Max LEDs now: " + std::to_string(_max_leds) + " Buffer size now: " + std::to_string(_data_buff_len));
        _sleds.push_back(led);

        return true;
    }

    //Get LED stripes number
    const std::size_t sleds() const {
        return _sleds.size();
    }

    /*
    * Stop working thread
    */
    virtual void stop() override {
      logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started.");
      piutils::Threaded::stop();
    }

    /*
    * Allocate data buffer
    */
    virtual bool initialize() override {
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Allocate memory");
        prepare_buffer();

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Start worker" );
        return piutils::Threaded::start<SLedCtrl>(this);
    }

    /*
    * Print device configuration
    */
    virtual const std::string printConfig() override {
        std::string result =  name() + " Maximun supported SLEDs: " + std::to_string(_max_sleds) + " Refresh delay (ms): " + std::to_string(get_refresh_delay().count()) + "\n";

        for (auto sled : _sleds){
            result += sled->printConfig();
        }
        return result;
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
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started Idx:" + std::to_string(_transf_idx));
        if(_transf.size() == 0 || _transf_idx >= _transf.size())
            return _transf_empty;

        return _transf.at(_transf_idx++);
    }

    /*
    * Add a new transformation to the queue
    */
    void transformation_add(const transf_type& transf){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started" );
        _transf.push_back(std::move(transf));
    }

    /*
    * Clear list of thransformations
    */
    void transformations_clear(){
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Started" );
        _transf.clear();
        _transf_idx = 0;
    }

    /*
    * Restart transformation queue
    */
    void transformations_restart(){
        for (auto transf  : _transf ){
           transf.second->reset();
       }

        _transf_idx = 0;
        cv.notify_one();
    }


    /*
    * TODO: Load list of transformations from the file
    */
    void load_transformation(const std::string& filename){

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
            const std::size_t blen = sled->get_buffer_length();

            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Procedd LED stripe with : " + std::to_string(lcount) + " Type: " + std::to_string(stp));

            const std::uint8_t* lgm = sled->gamma();
            const std::uint32_t* ldata = sled->leds_data();

            const size_t  dlen = get_data_length();
            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Initialize buffer Len: " + std::to_string(dlen));
            std::memset( (void*)_data_buff, 0, dlen);

            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Fill data buffer");
            for( std::size_t lidx = 0; lidx < lcount; lidx++ ){

                // Test purpose only. Convert 0RGB to R,G,B
                /*
		std::uint8_t rgb[3] = {
                    lgm[ (ldata[lidx] & 0xFF) ],
                    lgm[ ((ldata[lidx] >> 8 ) & 0xFF) ],
                    lgm[ ((ldata[lidx] >> 16 ) & 0xFF) ]
                };
		*/

                const int idx = lidx*3;
                _data_buff[idx + 0] = (ldata[lidx] & 0xFF);
                _data_buff[idx + 1] = ((ldata[lidx] >> 8 ) & 0xFF);
                _data_buff[idx + 2] = ((ldata[lidx] >> 16 ) & 0xFF);

	        //std::cout << "Idx: " << lidx << " Val: " << std::hex << ldata[lidx] << std::endl;

                //_data_buff[idx + 0] = 0x40;
                //_data_buff[idx + 1] = 0x20;
                //_data_buff[idx + 2] = 0x80;

            }

            if(!write_data(_data_buff, blen)){
                //Error on SPI level
                logger::log(logger::LLOG::ERROR, "LedCtrl", std::string(__func__) + " Could not write data");
                break;
            }
        }

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Switch channel Off" );
        //switch off stransmission channel
        this->Off();

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Delay" );
        //delay before new portion
        this->data_delay();

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Finished" );

    }

    /**
     * @brief If we put data to ourself we should make delay (500 ms) before send another part
     *
     */
    inline void data_delay() const {
        if(_refresh_delay.count()>0){
	   logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Refresh delay (ms): " + std::to_string(_refresh_delay.count()));
           // Wait 500 ms before sending new data
           std::this_thread::sleep_for(std::chrono::microseconds(_refresh_delay));
        }
    }

    /*
    * Apply transformation
    */
    const bool aplply_transformation(transf_type& transf){
        std::string led_idx = transf.first;
        bool applied = false;

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Transformation: " + transf.first);

        /*
        * Special processing for NOP transformation
        */
        if(transf.second->is_last()){
            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Last transformation: " + transf.first);
            /*
            * Send notification inside
            */
            transf.second->transform();
            return transf.second->is_finished();
        }

        for (auto sled  : _sleds )
        {
            /*
            * TODO: Check should we apply this transformation for this LED stripe?
            */

            transf.second->transform(sled->leds_data(), sled->leds());
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
        return (_transf_idx < _transf.size());
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
                auto transf = p->get_transformation();
                if( transf.second.get() == nullptr )
                    continue;

                logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Transformation detected " + transf.first );
                bool last_attempt = p->aplply_transformation(transf);

                while(!p->is_stop_signal() && !last_attempt){
                    last_attempt = p->aplply_transformation(transf);

                    logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Transformation. Last attempt: " + std::to_string(last_attempt) );
                }

            }
        }

        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Finished" );
    }


    void set_refresh_delay(std::chrono::microseconds rf_delay){
	_refresh_delay = rf_delay;
    }

    const std::chrono::microseconds get_refresh_delay() const {
	return _refresh_delay;
    }

private:
    std::vector<pled> _sleds;   //LED stripe list
    int _max_leds;              //Maximum number of LEDs
    int _max_sleds;             //maximun number of LED Stripes supported by controller

    std::uint8_t* _data_buff;   //Data buffer
    std::size_t _data_buff_len;

    std::vector<transf_type> _transf;
    std::size_t _transf_idx;

    std::chrono::microseconds _refresh_delay = std::chrono::microseconds(500);

    transf_type _transf_empty = std::make_pair(std::string(""), std::shared_ptr<pirobot::sled::SledTransformer>());
    /*
    *
    */
    const std::size_t get_data_length() const {
        return _data_buff_len;
    }

    void prepare_buffer() {
        if( _data_buff == nullptr ){
            logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Create data buffer: " + std::to_string(_data_buff_len) );
            _data_buff = (uint8_t*) std::malloc(sizeof(uint8_t)*_data_buff_len);
        }
    }

};

}//sledctrl
}//namespace item
}//namespace pirobot

#endif
