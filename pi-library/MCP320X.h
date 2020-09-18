/*
 * MCP320X.h
 * Support for MCP 3204/3208 (12-bit) and 3004/3008 (10-bit)  A/D Converters with SPI Serial Interface
 *
 *  Created on: Sep 06, 2017
 *  Updated for MCP300X Sep 10, 2020
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

enum MCP3XXX_DEVICE_TYPE : uint8_t {
    MCP3204 = 1,
    MCP3004 = 2,
    MCP3208 = 3,
    MCP3008 = 4
};

enum MCP3XXX_Bits : uint8_t {
    MCP32XX_Control_Start_Bit       = 0x04,
    MCP32XX_Control_SinDiff_Single  = 0x02,
    MCP32XX_Control_Null_Bit        = 0x10,
    MCP30XX_Control_Start_Bit       = 0x01,
    MCP30XX_Control_SinDiff_Single  = 0x80,
    MCP30XX_Control_Null_Bit        = 0x04
};

class MCP320X : public item::Item, public piutils::Threaded, public analogdata::AnalogDataProviderItf {

public:

    /*
    * Constructor.
    * GPIO is used for switch ON/OFF (CS/SHDN) Low - On, High - Off
    */
    MCP320X(const std::shared_ptr<pirobot::spi::SPI> spi,
        const std::shared_ptr<pirobot::gpio::Gpio> gpio,
        const std::string& name,
        const std::string& comment = "",
        MCP3XXX_DEVICE_TYPE _dev_type = MCP3XXX_DEVICE_TYPE::MCP3208,
        spi::SPI_CHANNELS channel = spi::SPI_CHANNELS::SPI_0,
        const unsigned int loop_delay = 5) :
            item::Item(gpio, name, comment, item::ItemTypes::AnlgDgtConvertor),
            m_spi(spi),
            m_channel(channel)
    {
        assert(get_gpio() != NULL);
        assert(get_gpio()->getMode() ==  gpio::GPIO_MODE::OUT);
        assert(spi);
        assert(!name.empty());

        if((_dev_type == MCP3XXX_DEVICE_TYPE::MCP3208) || (_dev_type == MCP3XXX_DEVICE_TYPE::MCP3008))
            m_anlg_inputs = 8;
        else
            m_anlg_inputs = 4;

        logger::log(logger::LLOG::DEBUG, "MCP320X", std::string(__func__) + " _dev_type: " + std::to_string(_dev_type) + " Inputs: " + std::to_string(m_anlg_inputs) + " Delay: " + std::to_string(loop_delay));

        set_loop_delay(loop_delay);

        Off(); ////switch device off
    }

    /*
    * Destructor
    */
    virtual ~MCP320X();

	virtual const std::string printConfig() override {
        std::string result =  name() + " SPI Channel: " + std::to_string(m_channel) + " Analog Inputs: " +
            std::to_string(inputs()) + "\n";
        for(int i = 0; i < inputs(); i++){
                result += (" channel: " + std::to_string(i) + (m_receivers[i] ? " ON " + m_receivers[i]->pname() : " OFF") + "\n");
        }

        return result;
    }

    static const int Max_Analog_Inputs = 8;

    /*
    *
    */
    virtual bool initialize() override{
        return piutils::Threaded::start<MCP320X>(this);
    }

    /*
    * Register data receiver
    */
    virtual bool register_data_receiver(const int input_idx, const std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf> receiver) noexcept(false) override;

    /*
    *
    */
    virtual const std::string pname() const override{
        return name();
    }


    inline virtual void stop() override;

    void On() {
        logger::log(logger::LLOG::DEBUG, "MCP320X", std::string(__func__) + " Switch to On");
        get_gpio()->Low();
    }

    void Off() {
        logger::log(logger::LLOG::DEBUG, "MCP320X", std::string(__func__) + " Switch to Off");
        get_gpio()->High();
    }

    bool activate_spi_channel(){
        return m_spi->set_channel_on(m_channel);
    }

    const bool is_active_agents() const {
        for(int i = 0; i < inputs(); i++){
            if(m_receivers[i] && m_receivers[i]->is_active())
                return true;
        }
        return false;
    }

    /**
     *
     */
    const uint16_t get_value(const int pin);

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
    MCP3XXX_DEVICE_TYPE _dev_type;
    pirobot::spi::SPI_CHANNELS m_channel;

public:
    std::shared_ptr<pirobot::analogdata::AnalogDataReceiverItf> m_receivers[Max_Analog_Inputs];
};

} //namespace mcp320x
} //namespace pirobot
#endif
