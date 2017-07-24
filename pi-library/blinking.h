/*
 * blinking.h
 *
 *  Created on: Jul 23, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_BLINKING_H_
#define PI_LIBRARY_BLINKING_H_

#include "Threaded.h"
#include "logger.h"
#include <wiringPi.h>

namespace pirobot {
namespace item {

const char TAG_[] = "Blink";

template<class T>
class Blinking: public piutils::Threaded {
public:
    Blinking(const T& item, 
        unsigned int tm_on=250, 
        unsigned int tm_off=500,
        unsigned int blinks = 10
        ) 
        : m_item(item), m_tm_on(tm_on), 
        m_tm_off(tm_off),m_blinks(blinks), m_on(false){

    	logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " Started with " + m_item.name());
    }

    virtual ~Blinking(){

    }

    unsigned int get_on() { return m_tm_on; }
    unsigned int get_off() { return m_tm_off; }
    unsigned int get_blinks() { return m_blinks; }

    void set(unsigned int tm_on=250, unsigned int tm_off=500, unsigned int blinks = 10){
        m_tm_on = tm_on;
        m_tm_off = tm_off;
        m_blinks = blinks;
    }

    void On() { m_on = true; }
    void Off() { m_on = false; }

    const bool is_on() const { return m_on; }

	virtual bool initialize() override {
    	logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " Started.");

    	return piutils::Threaded::start<Blinking>(this);
    }

	static void* worker(void* p){
	    logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " Worker started.");

	    Blinking* owner = static_cast<Blinking*>(p);

        unsigned int loop_delay = owner->get_loopDelay();
	    while(!owner->is_stop_signal()){
            int blinks = (owner->get_blinks() > 0 ? owner->get_blinks() : 10);
            while(owner->is_on() && blinks > 0){
                //Switch On
                owner->item_on();
                unsigned int tm_on = owner->get_on();
                //wait in loop with loop delay inerval
                while(owner->is_on() && tm_on > 0){
                    unsigned int tm_delay = (loop_delay > tm_on ? loop_delay : tm_on);
                    tm_on -= tm_delay;
                    delay(tm_delay);
                }
                //Switch Off
                owner->item_off();
                unsigned int tm_off = owner->get_off();
                //wait in loop with loop delay inerval
                while(owner->is_on() && tm_off > 0){
                    unsigned int tm_delay = (loop_delay > tm_off ? loop_delay : tm_off);
                    tm_off -= tm_delay;
                    delay(tm_delay);
                }

                blinks--;
                if(blinks == 0 && owner->get_blinks() == 0){
                    blinks = 10;
                }
            }

    		delay(loop_delay);
    	}

	    logger::log(logger::LLOG::DEBUG, TAG_, std::string(__func__) + " Worker finished.");
	    return (void*) 0L;
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
    T m_item;
};

} /* namespace item */
} /* namespace pirobot */

#endif
