/*
* AnalogLightMeter.h
* Support for Analog Light Meter
* I have HW5P-1
*
*  Created on: Sep 10, 2017
*      Author: Denis Kudia
*/

#ifndef PI_LIBRARY_AnalogLightMeter_H_
#define PI_LIBRARY_AnalogLightMeter_H_
  
#include <fstream>

#include "item.h"
#include "Threaded.h"
#include "AnalogDataReceiverItf.h"
#include "AnalogDataProviderItf.h"

#include "CircularBuffer.h"

namespace pirobot {
namespace anlglightmeter {

class AnalogLightMeter : public item::Item, public piutils::Threaded, public analogdata::AnalogDataReceiverItf{
public:
    AnalogLightMeter(
        const std::shared_ptr<pirobot::analogdata::AnalogDataProviderItf> provider,
        const std::string& name,
        const std::string& comment = "Analog Light Meter",
        const int analog_input_index=0,
        const unsigned short value_diff_for_event = 0, //do not generate events
        const bool debug_mode = false
        ):
        item::Item(name, comment, item::ItemTypes::AnalogMeter), 
        m_provider(provider),
        m_value_diff_for_event(value_diff_for_event),
        m_debug_size(2048),
        m_debug_values(nullptr),
        m_debug_data_counter(0)
    {
        assert(provider);

        set_idx(analog_input_index);
        m_buff = std::shared_ptr<piutils::circbuff::CircularBuffer<unsigned short>>(new piutils::circbuff::CircularBuffer<unsigned short>(20));

        if(m_provider){
            m_provider->register_data_receiver(get_idx(), std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf>(this));
        }

        if(debug_mode)
            activate_debug();
    }

    /*
    * Destructor
    */
    virtual ~AnalogLightMeter(){
        stop();

        if(m_debug_values){
            delete[] m_debug_values;
            m_debug_values = nullptr;
        }
    }

    /*
    * Provide callback for receiving data from analog provider
    * Free this call as quick as you can otherwise you will block next data reading 
    */
    virtual void data(const unsigned short value) override;

    /*
    *
    */    
    virtual const std::string pname() const override{
        return name();
    }
    
    virtual void activate() override{
        analogdata::AnalogDataReceiverItf::activate();
        m_provider->activate_data_receiver(get_idx());
    }


    /*
    *
    */
    virtual const std::string printConfig() override{
        if(m_provider)
            return name() + " Provider: " + m_provider->pname() + " Index: " + std::to_string(get_idx());

        return name();
    }        

    /*
    *
    */
    virtual bool initialize() override{
        return piutils::Threaded::start<AnalogLightMeter>(this);
    }

    virtual void stop() override;
    
    /*
    * Worker function
    */
    static void worker(AnalogLightMeter* p);
    
    const bool data_present() const {
        return !m_buff->is_empty();
    }

    const unsigned short get(){
        return m_buff->get();
    }

    /*
    * Return difference between measures for event generating
    */
    const unsigned short diff_for_event() const {
        return m_value_diff_for_event;
    }


    /*
    * Debug support
    */
    virtual void activate_debug() override {
        m_debug = true;
        m_debug_values = new unsigned short[m_debug_size];
    }

    void debug_save_value(unsigned short value){
        if(m_debug_data_counter<m_debug_size)
        m_debug_values[m_debug_data_counter++] = value;
    }
    
    virtual void unload_debug_data(const std::string& dest_type, const std::string& destination) override;
    

private:
    std::shared_ptr<piutils::circbuff::CircularBuffer<unsigned short>> m_buff;
    std::shared_ptr<pirobot::analogdata::AnalogDataProviderItf> m_provider;
    unsigned short m_value_diff_for_event;

    unsigned short* m_debug_values;
    int m_debug_data_counter;
    int m_debug_size;
};

}
}

#endif