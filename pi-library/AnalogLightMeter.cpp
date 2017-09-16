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

    //auto fn = [owner]{return (owner->is_stop_signal() || owner->is_read());};
    auto fn = [owner]{return (owner->is_stop_signal());};
    
    while(!owner->is_stop_signal()){
        //wait until stop signal will be received or we will have steps for processing
        {
            std::unique_lock<std::mutex> lk(owner->cv_m);
            owner->cv.wait(lk, fn);
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker. Signal detected.");
        }

    }
}

void AnalogLightMeter::data(const unsigned short value){
    unsigned short diff;
    if(m_idx==0){
        m_values[++m_idx] = value;
        diff = m_values[m_idx] - m_values[0];
    }
    else{
        m_values[--m_idx] = value;
        diff = m_values[m_idx] - m_values[1];
    }

    if(diff > 10){
        logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + 
            " Lights Metters Was: "  + std::to_string(m_values[(m_idx == 0 ? 1:0)]) + " New: " + std::to_string(m_values[m_idx]));    
    }
}


}//namespace anlglightmeter
}//namespace pirobot