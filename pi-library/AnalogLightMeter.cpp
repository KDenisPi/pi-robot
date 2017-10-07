/*
 * AnalogLightMeter.h
 * Support for Analog Light Meter
 * I have HW5P-1
 *
 *  Created on: Sep 10, 2017
 *      Author: Denis Kudia
 */
 #include "AnalogLightMeter.h"
 #include "logger.h"

namespace pirobot {
namespace anlglightmeter {
    
const char TAG[] = "AlgLtMt";
    
void AnalogLightMeter::stop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    piutils::Threaded::stop();
}

/*
 *
 */
 void AnalogLightMeter::worker(AnalogLightMeter* owner){
    std::string name = owner->name();
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker started. Name: " + name);

    unsigned short diff = 0;
    unsigned short prev_value = 0, value;
    int msg_counter = 0;

    auto fn = [owner]{return (owner->is_stop_signal() || (owner->is_active() && owner->data_present()));};
    
    while(!owner->is_stop_signal()){
        //wait until stop signal will be received or we will have steps for processing
        {
            std::unique_lock<std::mutex> lk(owner->cv_m);
            owner->cv.wait(lk, fn);

            /*
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + 
                " Worker. Signal detected. Stop: " + std::to_string(owner->is_stop_signal()) + 
                " Data: " + std::to_string(owner->data_present()) + 
                " Active: " + std::to_string(owner->is_active()));
            */
        }

        while(!owner->is_stop_signal() && owner->is_active() &&  owner->data_present()){
            value = owner->get();
            msg_counter++;
            diff = (value > prev_value ? value - prev_value : prev_value - value);
            
            if(diff > owner->diff_for_event()){
                logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " " + name +
                " Was: "  + std::to_string(prev_value) + " New: " + std::to_string(value));
            }
            prev_value = value;
        }
        /*
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + 
            " Worker. Loop finished: " + std::to_string(owner->is_stop_signal()) + 
            " Data: " + std::to_string(owner->data_present()) + 
            " Active: " + std::to_string(owner->is_active()));
        */
        if(!owner->is_active()){
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished. Name: " + name + 
            " Processed msg: " + std::to_string(msg_counter));
                
        }
    }
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished. Name: " + name + 
        " Processed msg: " + std::to_string(msg_counter));
}

void AnalogLightMeter::data(const unsigned short value){
    const bool notify = m_buff->is_empty();
    m_buff->put(value);
    if(notify)
        cv.notify_one();
}


}//namespace anlglightmeter
}//namespace pirobot