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

    // 0 - current value, 1 - previous value, 2 - difference
    unsigned short values[3] = {0,0,0};
    int msg_counter = 0;

    auto fn = [owner]{return (owner->is_stop_signal() || (owner->is_active() && owner->data_present()));};

    while(!owner->is_stop_signal()){
        //wait until stop signal will be received or we will have steps for processing
        {
            std::unique_lock<std::mutex> lk(owner->cv_m);
            owner->cv.wait(lk, fn);
        }

        while(!owner->is_stop_signal() && owner->is_active() &&  owner->data_present()){
            values[0] = owner->get();
            msg_counter++;

            if(values[1] == 0){
              values[1] = values[0];
              continue;
            }

            values[2] = (values[0] > values[1] ? values[0] - values[1] : values[1] - values[0]);

            if(owner->is_debug()){
                owner->debug_save_value(values[0]);
            }

            //ignore first measure
            if(owner->diff_for_event() > 0 && values[1] > 0 && values[2] > owner->diff_for_event()){

                if(owner->is_debug()){
                   logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " " + name +
                    " Was: "  + std::to_string(values[1]) + " New: " + std::to_string(values[0]) +
                    " (" + std::to_string(values[2]) + ") Counter: " + std::to_string(msg_counter));
                }

                if(owner->notify){
                    owner->notify(owner->type(), name, (void*)values);
                }

                values[1] = values[0];
            }
        }

        if(!owner->is_active()){
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished. Name: " + name +
            " Processed msg: " + std::to_string(msg_counter));

        }
    }
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished. Name: " + name +
        " Processed msg: " + std::to_string(msg_counter));
}

/*
* Put value to buffer and notify working thread
*/
void AnalogLightMeter::data(const unsigned short value){
    const bool notify = m_buff->is_empty();
    m_buff->put(value);
    if(notify)
        cv.notify_one();
}

/*
* Upload debug data to file with format number,value (for future using in Excel)
*/
void AnalogLightMeter::unload_debug_data(const std::string& dest_type, const std::string& destination){
    if(dest_type == "file"){
        std::fstream s(destination, s.binary | s.trunc | s.out);
        if (s.is_open()) {
          for(int i = 0; i < m_debug_data_counter; i++)
            s << i << "," << m_debug_values[i] << std::endl;
          s.close();
        }
        else
            logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not unload data to file " + destination);
    }
}

}//namespace anlglightmeter
}//namespace pirobot
