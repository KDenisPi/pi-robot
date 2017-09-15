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

namespace pirobot {
namespace anlglightmeter {

class AnalogLightMeter : public item::Item, public piutils::Threaded, public AnalogDataReceiverItf{
public:
    AnalogLightMeter(const std::shared_ptr<pirobot::AnalogDataProviderItf> provider, 
        const std::string name):
        item::Item(name, "Analog Light Meter", item::ItemTypes::AnalogMeter), m_provider(provider),
        m_values({0,0}), m_idx(0)
    {

    }
    virtual ~AnalogLightMeter() {}

    /*
    * Provide callback for receiving data from analog provider
    * Free this call as quick as you can otherwise you will block next data reading 
    */
    virtual void data(const unsigned short value) override{
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
            std::out << " Lights Metters Was: " << m_values[(m_idx == 0 ?1:0)] " New: " << m_values[m_idx] << std::endl;    
        }
    }

private:
    std::shared_ptr<pirobot::AnalogDataProviderItf> m_provider;

    unsigned short m_values[2];
    int m_idx;
};

}
}

#endif