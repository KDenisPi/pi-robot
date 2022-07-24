/*
 * Threaded.cpp
 *
 *  Created on: Dec 4, 2016
 *      Author: Denis Kudia
 */

#include "Threaded.h"
#include "logger.h"

namespace piutils {

Threaded::Threaded() : m_stopSignal(false), m_loop_delay(100)
{
}

Threaded::~Threaded() {
    // TODO Auto-generated destructor stub
}


void Threaded::stop (const bool set_stop){
    if(set_stop && !is_stopped()){
        logger::log(logger::LLOG::DEBUG, TAG_TR, std::string(__func__) + std::string(" Stop signal sent. Thread:") + get_thread_id_str());
        set_stop_signal(true);
    }

    //std::cout <<  "Threaded::stop " << get_thread_id_str() << " is_stopped " << is_stopped() << std::endl;

    if( !is_stopped() ){
        logger::log(logger::LLOG::DEBUG, TAG_TR, std::string(__func__) + std::string(" Wait.. Thread: ") + get_thread_id_str());
        this->m_thread.join();
        //std::cout <<  "Threaded::stop " << get_thread_id_str() << " join() finished " << std::endl;
    }
    else{
        logger::log(logger::LLOG::DEBUG, TAG_TR, std::string(__func__) + std::string(" Finished already. Thread: ") + get_thread_id_str());
    }

    //std::cout <<  " Finished Thread:" + get_thread_id_str() << std::endl;
    logger::log(logger::LLOG::DEBUG, TAG_TR, std::string(__func__) + " Finished Thread:" + get_thread_id_str());
}


} /* namespace pirobot */
