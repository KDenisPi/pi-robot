/*
 * blinking.h
 *
 *  Created on: Jul 23, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_BLINKING_H_
#define PI_LIBRARY_BLINKING_H_

#include "item.h"
#include "Threaded.h"
#include "logger.h"
#include <wiringPi.h>

namespace pirobot {
namespace item {

const char TAG_[] = "Blink";

template<class T>
class Blinking: public Item, public piutils::Threaded {
public:
    Blinking(const std::shared_ptr<T>& item,
        const std::string name,
        const std::string comment = "",
        unsigned int tm_on=250,
        unsigned int tm_off=500,
        unsigned int blinks = 10
        )
        : Item(item->get_gpio(), name, comment, ItemTypes::BLINKER),
        m_item(item), m_tm_on(tm_on),
        m_tm_off(tm_off),m_blinks(blinks), m_on(false){

        logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " " + this->name() + " Started with " + m_item->name());
    }

    /*
    *
    */
    virtual ~Blinking(){

    }

    virtual const std::string to_string() override {
        return name();
    }

    virtual const std::string printConfig() override {
        return name();
    }

    /*
    *
    */
    unsigned int get_on() { return m_tm_on; }
    unsigned int get_off() { return m_tm_off; }
    unsigned int get_blinks() { return m_blinks; }

    void set(unsigned int tm_on=250, unsigned int tm_off=500, unsigned int blinks = 10){
        m_tm_on = tm_on;
        m_tm_off = tm_off;
        m_blinks = blinks;
    }

    void On(){
       m_on = true;
       cv.notify_one();
     }

    void Off() { m_on = false; }

    const bool is_on() const { return m_on; }

    virtual bool initialize() override {
       logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " Started.");
       return piutils::Threaded::start<Blinking>(this);
    }

    //
    //
    //
    inline void wait_loop(const unsigned int tm_interval){
        unsigned int _tm = tm_interval;
        unsigned int loop_delay = get_loop_delay();

        //If interval less than 2 sec - wait full interval
        //wait in loop with loop delay inerval
        if(tm_interval < 2000){
            std::this_thread::sleep_for(std::chrono::milliseconds(tm_interval));
        }
        else{
            while(is_on() && _tm > 0){
                unsigned int tm_delay = (loop_delay > _tm ? loop_delay : _tm);
                _tm -= tm_delay;
                std::this_thread::sleep_for(std::chrono::milliseconds(tm_delay));
            }
        }
    }

    /*
    * Blinking worker function
    */
    static void worker(Blinking* owner){
        logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " Worker started. " + owner->to_string());

        unsigned int loop_delay = owner->get_loop_delay();
        std::string name = owner->name();

        auto fn = [owner]{return (owner->is_stop_signal() || owner->is_on());};

        while(!owner->is_stop_signal()){

            {
                std::unique_lock<std::mutex> lk(owner->cv_m);
                owner->cv.wait(lk, fn);
            }

            logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " Blinking started. ");

            int blinks = (owner->get_blinks() > 0 ? owner->get_blinks() : 10);
            while(!owner->is_stop_signal() && owner->is_on() && blinks > 0){
                    //Switch On
                    owner->item_on();

                    unsigned int tm_on = owner->get_on();
                    owner->wait_loop(tm_on);

                    //Switch Off
                    owner->item_off();
                    unsigned int tm_off = owner->get_off();
                    owner->wait_loop(tm_off);

                    blinks--;
                    //for infinity loops always increment counter
                    if(blinks == 0 && owner->get_blinks() == 0){
                        blinks = 10;
                    }
            }

           //check if current loop is finished and send signal to parent
           if((owner->is_on() && blinks == 0)){
             owner->Off(); //switch status to  off
              if(owner->notify){
                 unsigned int state = GENERAL_NTFY::GN_DONE;
                 owner->notify(owner->type(), name, (void*)(&state));
              }
           }

           std::this_thread::sleep_for(std::chrono::milliseconds(loop_delay));
        }

        logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " Worker finished. " + owner->to_string());
    }

    virtual void stop() override {
       logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " Started.");
       Off();
       piutils::Threaded::stop();
    }

    void item_on(){
        m_item->On();
    }

    void item_off(){
        m_item->Off();
    }

private:
    unsigned int m_tm_on;
    unsigned int m_tm_off;
    unsigned int m_blinks;
    bool m_on;
    std::shared_ptr<T> m_item;
};

} /* namespace item */
} /* namespace pirobot */

#endif
