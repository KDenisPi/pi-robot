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
        const std::string name,
        const std::string comment = "Analog Light Meter",
        const int analog_input_index=0
        ):
        item::Item(name, comment, item::ItemTypes::AnalogMeter), 
        m_provider(provider),
        m_idx(analog_input_index)
    {
        assert(provider);
        
        m_values[0] = 0; 
        m_values[1] = 0; 

        if(m_provider){
            m_provider->register_data_receiver(m_idx, std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf>(this));
        }
    }

    virtual ~AnalogLightMeter(){
        stop();
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
    
    /*
    *
    */
	virtual const std::string printConfig() override{
        if(m_provider)
            return name() + " Provider: " + m_provider->pname() + " Index: " + std::to_string(m_idx);

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
    

private:
    std::shared_ptr<pirobot::analogdata::AnalogDataProviderItf> m_provider;

    unsigned short m_values[2];

    //Analog Input assigned for this device
    int m_idx;
};

}
}

#endif