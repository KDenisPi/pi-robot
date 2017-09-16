/*
 * MCP320X.cpp
 * Support for MCP 3204/3208 A/D Converters with SPI Serial Interface
 *
 *  Created on: Sep 06, 2017
 *      Author: Denis Kudia
 */

#include "MCP320X.h"
#include "logger.h"

namespace pirobot {
namespace mcp320x {
    
const char TAG[] = "MCP320x";

/*
* Destructor
*/
MCP320X::~MCP320X() {
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));

    //switch device off
    get_gpio()->High();

    stop();
}

void MCP320X::stop(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");
    piutils::Threaded::stop();
}

/*
 *
 */
 void MCP320X::worker(MCP320X* owner){
    std::string name = owner->name();
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker started. Name: " + name);

    auto fn = [owner]{return (owner->is_stop_signal() || owner->is_read());};
    auto fn_read = [owner]{return (!owner->is_stop_signal() && owner->is_read());};
    
    while(!owner->is_stop_signal()){
        //wait until stop signal will be received or we will have steps for processing
        {
            std::unique_lock<std::mutex> lk(owner->cv_m);
            owner->cv.wait(lk, fn);
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker. Signal detected.");
        }

        if(owner->is_stop_signal())
            break;

        //activate my SPI channel    
        owner->activate_spi_channel();

        /*
        * Should I switch MCP320X after EACH reading or not? 
          I do not think so. 
        
        3.7 Chip Select/Shutdown (CS/SHDN)
            The CS/SHDN pin is used to initiate communication
            with the device when pulled low and will end a
            conversion and put the device in low power standby
            when pulled high. The CS/SHDN pin must be pulled
            high between conversions.        
        */

        //switch device On    
        owner->On();
        

        auto fn = [owner](unsigned char* buff, const unsigned char in_pin) -> unsigned short {
            buff[0] = (Control_Start_Bit | Control_SinDiff_Single | (in_pin >> 2));
            buff[1] = (in_pin << 6);
            buff[2] = 0x00;

            //request data from device
            int res = owner->data_rw(buff, 3);
            //conver result to 12-bit  unsigned value
            return (unsigned short)((((buff[1] & 0x0F) << 8) | buff[2]));
        };

        unsigned short value;
        unsigned char buff[3];
        while(fn_read()){

            /*
            * Main loop - read analog inputs through SPI
            */

            unsigned char input_num = 0;
            for(int i = 0; i < owner->inputs(); i++){
                if(owner->m_receivers[i]){
                    value = fn(buff, (unsigned char)i);
                    owner->m_receivers[i]->data(value);                    
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        //switch device Off    
        owner->Off();
    }
}

void MCP320X::stop_read(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));
    std::lock_guard<std::mutex> lk(cv_m);
    m_read_flag = false;
}

void MCP320X::start_read(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));
    std::lock_guard<std::mutex> lk(cv_m);
    m_read_flag = true;
    cv.notify_one();
}

/*
* Register data receiver 
*/
bool MCP320X::register_data_receiver(const int input_idx, 
    const std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf> receiver){

    if(input_idx >= m_anlg_inputs){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + 
                " Invalid channel number Idx: " + std::to_string(input_idx));
        throw std::runtime_error(std::string("Invalid channel number"));
    }

    if(!receiver){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid receiver object");
        throw std::runtime_error(std::string("Invalid receiver object"));
    }
             
    m_receivers[input_idx] = receiver;

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + 
            " Added data received " + receiver->pname() + " for input: " + std::to_string(input_idx));
    return true;
}


} //namespace mcp320x
} //namespace pirobot

