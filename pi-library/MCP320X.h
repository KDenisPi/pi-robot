/*
 * MCP320X.h
 * Support for MCP 3204/3208 A/D Converters with SPI Serial Interface
 *
 *  Created on: Sep 06, 2017
 *      Author: Denis Kudia
 */

 #ifndef PI_LIBRARY_MCP320X_H_
 #define PI_LIBRARY_MCP320X_H_
  
#include "item.h"
#include "Threaded.h"
#include "SPI.h" 
#include "AnalogDataProviderItf.h"

namespace pirobot {
namespace mcp320x {

#define MAX_NUMBER_ANALOG_INPUTS 8

enum MCP320X_INPUTS {
    MCP3204 = 4,
    MCP3208 = 8
};

#define Control_Start_Bit 0x04
#define Control_SinDiff_Single 0x02
#define Control_SinDiff_Diff 0x00

class MCP320X : public item::Item, public piutils::Threaded, public analogdata::AnalogDataProviderItf {

public:

    /*
    * Constructor.
    * GPIO is used for switch ON/OFF (CS/SHDN) Low - On, High - Off
    */
    MCP320X(const std::shared_ptr<pirobot::spi::SPI> spi, 
        const std::shared_ptr<pirobot::gpio::Gpio> gpio, 
        const std::string name,
        const std::string comment = "",
        MCP320X_INPUTS anlg_inputs = MCP320X_INPUTS::MCP3208,
        spi::SPI_CHANNELS channel = spi::SPI_CHANNELS::SPI_0) : 
            item::Item(gpio, name, comment, item::ItemTypes::AnlgDgtConvertor),
            m_spi(spi),
            m_anlg_inputs((int)anlg_inputs),
            m_channel((int)channel)
    {
        assert(get_gpio() != NULL);
        assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);
        assert(spi);
        assert(!name.empty());
        
        //switch device off
        get_gpio()->High();
    }

    /*
    * Destructor
    */
    virtual ~MCP320X();

	virtual const std::string printConfig() override{
        std::string result =  name() + " SPI Channel: " + std::to_string(m_channel) + " Analog Inputs: " + 
            std::to_string(m_anlg_inputs) + "\n";
        for(int i = 0; i < m_anlg_inputs; i++){
                result += (" channel: " + std::to_string(i) + (m_receivers[i]? "ON" : "OFF") + "\n");
        }

        return result;
    }
    
    /*
    *
    */
    virtual bool initialize() override{
        return piutils::Threaded::start<MCP320X>(this);
    }
    
    /*
    * Register data receiver 
    */
    virtual bool register_data_receiver(const int input_idx, 
        const std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf> receiver) noexcept(false) override;

    /*
    *
    */    
    virtual const std::string pname() const override{
        return name();
    }
    

    inline virtual void stop() override;

    void On() {
        get_gpio()->Low();
    }

    void Off() {
        get_gpio()->High();
    }

    void activate_spi_channel(){
        m_spi->set_channel_on(m_channel);
    }

    const bool is_active_agents() const {
        for(int i = 0; i < inputs(); i++){
            if(m_receivers[i] && m_receivers[i]->is_active())
                return true;
        }
        return false;
    }

    /*
    * Worker function
    */
    static void worker(MCP320X* p);
    
    /*
    * Read data from device over SPI bus
    */
    inline int data_rw(unsigned char* data, int len){
        m_spi->data_rw(data, len);
    }

    const int inputs() const {return m_anlg_inputs;}
    virtual void activate_data_receiver(const int input_idx) override;
    

private:
    std::shared_ptr<pirobot::spi::SPI> m_spi;
    int m_anlg_inputs;
    int m_channel;

public:
    std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf> m_receivers[MAX_NUMBER_ANALOG_INPUTS];
};

} //namespace mcp320x
} //namespace pirobot
#endif