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

namespace pirobot {
namespace mcp320x {

enum MCP320X_INPUTS {
    MCP3204 = 4,
    MCP3208 = 8
};
    
class MCP320X : public item::Item, public piutils::Threaded {

public:

    /*
    * Constructor.
    * GPIO is used for switch ON/OFF (CS/SHDN) Low - On, High - Off
    */
    MCP320X(const std::shared_ptr<pirobot::spi::SPI> spi, 
        const std::shared_ptr<pirobot::gpio::Gpio> gpio, 
        const std::string name,
        MCP320X_INPUTS anlg_inputs = MCP320X_INPUTS::MCP3208,
        spi::SPI_CHANNELS channel = spi::SPI_CHANNELS::SPI_0,
        const std::string comment = "") : 
            item::Item(gpio, name, comment, item::ItemTypes::AnlgDgtConvertor),
            m_spi(spi),
            m_anlg_inputs((int)anlg_inputs),
            m_channel((int)channel),
            m_read_flag(false)
    {
        assert(get_gpio() != NULL);
        assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);
        assert(spi);
        assert(!name.empty());
    
        get_gpio()->High();
    }

    /*
    * Destructor
    */
    virtual ~MCP320X();

	virtual const std::string printConfig() override{
        return name() + " SPI Channel: " + std::to_string(m_channel) + " Analog Inputs: " + std::to_string(m_anlg_inputs);
    }
    
    /*
    *
    */
    virtual bool initialize() override{
        return piutils::Threaded::start<MCP320X>(this);
    }
    
    inline virtual void stop() override;

    inline void stop_read();
    inline void start_read();
    
    const bool is_read() const{
        return m_read_flag;
    }

    void On() {
        get_gpio()->Low();
    }

    void Off() {
        get_gpio()->High();
    }

    void activate_spi_channel(){
        m_spi->set_channel_on(m_channel);
    }
    /*
    * Worker function
    */
    static void worker(MCP320X* p);
    

private:
    std::shared_ptr<pirobot::spi::SPI> m_spi;
    int m_anlg_inputs;
    int m_channel;
    bool m_read_flag;
};

} //namespace mcp320x
} //namespace pirobot
#endif