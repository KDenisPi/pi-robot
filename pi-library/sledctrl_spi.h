/*
 * sledctrl_spi.h
 *
 * SPI based LED Stripe Controller
 *
 *  Created on: Jan 11, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SLED_CTRL_SPI_H_
#define PI_LIBRARY_SLED_CTRL_SPI_H_

#include "SPI.h"
#include "sledctrl.h"

namespace pirobot {
namespace item {
namespace sledctrl {

class SLedCtrlSpi : public pirobot::item::sledctrl::SLedCtrl
{
public:
    SLedCtrlSpi(const std::shared_ptr<pirobot::spi::SPI> spi,
        const int sleds,
        const std::string& name,
        const std::string& comment="",
        spi::SPI_CHANNELS channel = spi::SPI_CHANNELS::SPI_0) :
            SLedCtrl(item::ItemTypes::SLEDCRTLSPI, sleds, name, comment), _spi(spi), _spi_channel(channel)
    {

        logger::log(logger::LLOG::DEBUG, "LedCtrlSpi", std::string(__func__) + " name " + name);
    }

    virtual bool write_data(unsigned char* data, int len) override {
        logger::log(logger::LLOG::DEBUG, "LedCtrl", std::string(__func__) + " Write to SPI: " +  std::to_string(len));

        //Write data to SPI
        return _spi->data_rw(data, len);
    }


    /*
    * Print device configuration
    */
    virtual const std::string printConfig() override {
        std::string result =  name() + " SPI Channel: " + std::to_string(_spi_channel) + "\n";

        result += SLedCtrl::printConfig();

        return result;
    }

    /*
    * ON for SPI based device
    */
    virtual void On() override{
        _spi->set_channel_on( _spi_channel );
    }

    /*
    * OFF for SPI base device
    */
    virtual void Off() override{
        _spi->set_channel_off( _spi_channel );
    }


private:

    std::shared_ptr<pirobot::spi::SPI> _spi;
    int _spi_channel;
};


}
}
}

#endif