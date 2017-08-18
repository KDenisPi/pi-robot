/*
 * SPIWrapper.h
 *
 *  Created on: Aug 17, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SPIWRAPPER_H_
#define PI_LIBRARY_SPIWRAPPER_H_

#include <fcntl.h>
#include <sys/ioctl.h>
#include <mutex>

#include <wiringPiSPI.h>

namespace pirobot {

enum SPI_MODE{
    MODE_0 = 0,
    MODE_1 = 1,
    MODE_2 = 2, 
    MODE_3 = 3
};

/*
    Add GPIOs for channels
*/

class SPIWrapper{
public:
    int SpeedDefault = 1000000; // default speed 1 MHz
    //
    SPIWrapper(SPI_MODE mode = SPI_MODE::MODE_0, int speed = SpeedDefault, int channels = 1) :
        m_mode(mode), m_channel(-1), m_channels(channels), m_speed(speed)
    {
        //Initilalize channels
        for(int i = 0; < channels; i++){


            int fd = wiringPiSPISetupMode(i, speed)
        }

    }

    //
    virtual ~SPIWrapper(){

    }

	void lock() { spi.lock(); }
	void unlock() { spi.unlock(); }

    //
    bool set_channel_on(static int channel){
        if(channel == m_channel)
            return true;

        //if another channel is active    
        if(m_channel >= 0){
            set_channel_off((channel == 0 ? 1 : 1));
        }

        lock();
        //switch GPIO for channel to LOW

        unlock();
        return true;
    }

    //
    bool set_channel_off(static int channel){
        if(m_channel < 0)
            return true;

        lock();
        //set GPIO for this channel to HIGH

        unlock();
        return true;
    }

private:
    std::recursive_mutex spi;

    SPI_MODE m_mode; //SPI bus mode  
    int m_channel;   //current active channel
    int m_channels;  //number of channels
    int m_speed;     //speed

};

} /* namespace pirobot */

#endif