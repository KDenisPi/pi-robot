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
    Off();
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

    auto fn_start = [owner]{return (owner->is_stop_signal() || owner->is_active_agents());};
    auto fn_read = [owner]{return (!owner->is_stop_signal() && owner->is_active_agents());};

    while(!owner->is_stop_signal()){
        //wait until stop signal will be received or we will have steps for processing
        {
            std::unique_lock<std::mutex> lk(owner->cv_m);
            owner->cv.wait(lk, fn_start);
            logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker. Signal detected.");
        }

        if(owner->is_stop_signal())
            break;

        //activate my SPI channel
        if(!owner->activate_spi_channel()){
            //could not activate channel
            continue;
        }


        auto fn_is_active_agent = [owner](const int idx) -> bool {
            return (owner->m_receivers[idx] && owner->m_receivers[idx]->is_active());
        };

        unsigned short value;
        while(fn_read()){

            //switch device On
            owner->On();

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            /*
            * Main loop - read analog inputs through SPI
            */
            unsigned char input_num = 0;
            for(int i = 0; i < owner->inputs(); i++){
                if(fn_is_active_agent(i)){
                    value = owner->get_value(i);
                    if(value != 0xFFFF){
                        owner->m_receivers[i]->data(value);
                    }
                }
            }

            //switch device Off
            owner->Off();

            std::this_thread::sleep_for(std::chrono::milliseconds(owner->get_loop_delay()));
        }
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished. Name: " + name);
}


/**
 *
 */
const uint16_t MCP320X::get_value(const int pin){

    unsigned char buff[3];

        /*
        * Should I switch MCP320X after EACH reading or not? - Yes


        3.7 Chip Select/Shutdown (CS/SHDN)
            The CS/SHDN pin is used to initiate communication
            with the device when pulled low and will end a
            conversion and put the device in low power standby
            when pulled high. The CS/SHDN pin must be pulled
            high between conversions.
        */


        /**
         * Read function for MCP32XX (12-bit)
         */
        auto fn_read_data_mcp32xx = [this](unsigned char* buff, const unsigned char in_pin) -> unsigned short {
            buff[0] = (MCP32XX_Control_Start_Bit | MCP32XX_Control_SinDiff_Single | (in_pin >> 2));
            buff[1] = (in_pin << 6);
            buff[2] = 0x00;

            //request data from device
            bool res = data_rw(buff, 3);

            // Valide Null-bit Just in case
            if((buff[1] & MCP32XX_Control_Null_Bit) != 0){
                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Null bit is not NULL");
                return 0;
            }

            //conver result to 12-bit  unsigned value
            return (unsigned short)((((buff[1] & 0x0F) << 8) | buff[2]));
        };

        /**
         * Read function for MCP30XX (10-bit)
         */
        auto fn_read_data_mcp30xx = [this](unsigned char* buff, const unsigned char in_pin) -> unsigned short {
            buff[0] = MCP30XX_Control_Start_Bit;
            buff[1] = (MCP30XX_Control_SinDiff_Single | (in_pin << 4));
            buff[2] = 0x00;

            //request data from device
            bool res = data_rw(buff, 3);

            // Valide Null-bit Just in case
            if((buff[1] & MCP30XX_Control_Null_Bit) != 0){
                logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Null bit is not NULL");
                return 0;
            }

            //conver result to 10-bit  unsigned value
            return (unsigned short)((((buff[1] & 0x03) << 8) | buff[2]));
        };

        uint16_t value = 0xFFFF;
        if(m_receivers[pin]->before()){
            value = fn_read_data_mcp32xx(buff, (unsigned char)pin);
            m_receivers[pin]->after();
        }

        return value;
}

void MCP320X::activate_data_receiver(const int input_idx){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Received activate signal from: " + std::to_string(input_idx));
    cv.notify_one();
}

/*
* Register data receiver
*/
bool MCP320X::register_data_receiver(const int input_idx, const std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf> receiver){

    if(input_idx >= m_anlg_inputs){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Invalid channel number Idx: " + std::to_string(input_idx));
        return false;
    }

    if(!receiver){
        logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + " Release receiver object");
        m_receivers[input_idx].reset();
        return true;
    }

    m_receivers[input_idx] = receiver;

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Added data received " + receiver->pname() + " for input: " + std::to_string(input_idx));
    return true;
}


} //namespace mcp320x
} //namespace pirobot

