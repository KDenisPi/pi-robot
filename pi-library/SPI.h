/*
 * SPI.h
 *
 *  Created on: Aug 17, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SPI_H_
#define PI_LIBRARY_SPI_H_

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <mutex>
#include <functional>

#include <wiringPiSPI.h>

#include "provider.h"
#include "gpio.h"

namespace pirobot {
namespace spi {

enum SPI_MODE{
    MODE_0 = 0,
    MODE_1 = 1,
    MODE_2 = 2, 
    MODE_3 = 3
};

#define SPI_SpeedDefault 1000000

struct SPI_config {
    int channels;
    int speed[2] = {SPI_SpeedDefault, SPI_SpeedDefault};
    SPI_MODE mode[2] = {SPI_MODE::MODE_0, SPI_MODE::MODE_0};

    SPI_config(){
        channels = 1;
    }
};

/*
* Add GPIOs for channels
*/
class SPI :  public pirobot::provider::Provider{
public:
    //
    SPI(const std::string& name, const struct SPI_config& config) :
        Provider(pirobot::provider::PROVIDER_TYPE::PROV_SPI, name), 
        m_channel(-1), m_channels(config.channels)
    {
        //Initilalize channels
        for(int i = 0; i < m_channels; i++){
            m_mode[i] = config.mode[i];
            m_speed[i] = config.speed[i];
            int fd = wiringPiSPISetupMode(i, config.speed[i], config.mode[i]);
            //TODO: Check result and generate exception if needed
        }

    }

    //
    virtual ~SPI(){
        //Close SPI channels
        for(int i = 0; i < m_channels; i++){
            int fd = wiringPiSPIGetFd(i);
            close(fd);
        }
    }

    void lock() { spi.lock(); }
    void unlock() { spi.unlock(); }

    //
    bool set_channel_on(const int channel){
        if(channel == m_channel)
            return true;

        //if another channel is active    
        if(m_channel >= 0){
            set_channel_off((channel == 0 ? 1 : 0));
        }

        lock();
        //switch GPIO for channel to LOW
        if(m_gpio[channel]){
            m_gpio[channel]->set_level(gpio::SGN_LEVEL::SGN_LOW);
        }
        unlock();

        return true;
    }

    //
    bool set_channel_off(const int channel){
        if(m_channel < 0)
            return true;

        lock();
        //set GPIO for this channel to HIGH
        if(m_gpio[channel]){
            m_gpio[channel]->set_level(gpio::SGN_LEVEL::SGN_HIGH);
        }
        unlock();

        return true;
    }

private:
    std::recursive_mutex spi;

    SPI_MODE m_mode[2]; //SPI bus mode  
    int m_channel;      //current active channel
    int m_channels;     //number of channels
    int m_speed[2];     //speed
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio[2];
};

} //namespace spi
} /* namespace pirobot */

#endif