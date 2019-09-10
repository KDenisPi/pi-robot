/*
 * SPI.h
 *
 *  Created on: Aug 17, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SPI_H_
#define PI_LIBRARY_SPI_H_

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <mutex>
#include <functional>

#include "logger.h"
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

enum SPI_CHANNELS {
    SPI_0 = 0,
    SPI_1 = 1
};

#define SPI_SpeedDefault 1000000 //1Mhz

struct SPI_config {
    int channels;
    int speed[2] = {SPI_SpeedDefault, SPI_SpeedDefault};
    SPI_MODE mode[2] = {SPI_MODE::MODE_0, SPI_MODE::MODE_0};
    bool real_world = true;

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
    SPI(const std::string& name, const struct SPI_config& config,
        std::shared_ptr<pirobot::gpio::Gpio> ce0,
        std::shared_ptr<pirobot::gpio::Gpio> ce1) :
        Provider(pirobot::provider::PROVIDER_TYPE::PROV_SPI, name),
        m_channel(-1), m_channels(config.channels), m_real_world(config.real_world)
    {
        assert((m_channels <= 2) && (m_channels > 0));

        _fd[0] = _fd[1] = -1;
        m_gpio[0] = ce0;
        m_gpio[1] = ce1;

        //Initilalize channels
        for(int i = 0; i < m_channels; i++){
            m_mode[i] = config.mode[i];
            m_speed[i] = config.speed[i];
            if(m_real_world)
                int fd = setup_mode((SPI_CHANNELS)i, config.speed[i], config.mode[i]);
            //TODO: Check result and generate exception if needed
        }

        //switch OFF by default
        Off();
    }

    //TODO: Implemet SPI provider
    int setup_mode(const SPI_CHANNELS channel, const int speed, const SPI_MODE mode){
        return -1;
    }

    //
    virtual ~SPI(){
        //Close SPI channels
        for(int i = 0; i < m_channels; i++){
            //int fd = wiringPiSPIGetFd(i);
            if(_fd[i] > 0)
                close(_fd[i]);
        }
    }

    inline void lock() { spi_mtx.lock(); }
    inline void unlock() {spi_mtx.unlock(); }

    /*
    *
    */
    inline void Off(){
        for(int i = 0; i < m_channels; i++)
            set_channel_off(i);
    }

    //
    bool set_channel_on(const int channel) noexcept(false){

        logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " ON Channel: " + std::to_string(channel));
        if(channel >= m_channels){
            throw std::runtime_error(std::string("Invalid channel number"));
        }

        if(channel == m_channel)
            return true;

        //if another channel is active
        if(m_channel >= 0){
            set_channel_off((channel == 0 ? 1 : 0));
        }

        //switch GPIO for channel to LOW
        if(!m_gpio[channel]){
            logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " ON already. Channel: " + std::to_string(channel));
            return false;
       }

        lock();
        m_channel = channel;
        m_gpio[channel]->Low();
        unlock();

        logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " ON Done. Channel: " + std::to_string(channel));
        return true;
    }

    //
    bool set_channel_off(const int channel) noexcept(false){
        logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " OFF Channel: " + std::to_string(channel));
        if(m_channel < 0)
            return true;

        if(channel >= m_channels){
            throw std::runtime_error(std::string("Invalid channel number"));
        }

        //set GPIO for this channel to HIGH
        if(m_gpio[channel])
            return false;

        lock();
        m_gpio[channel]->High();
        unlock();

        return true;
    }

    /*
    * TODO:
    */
    int data_read_write(const int channel, unsigned char* data, int len){
        return 0;
    }

    /*
    * Read data from device over SPI bus
    */
    bool data_rw(unsigned char* data, int len){
        if(m_real_world){
            //int ret = wiringPiSPIDataRW(m_channel, data, len);
            int ret = data_read_write(m_channel, data, len);
        }
        else{
            data_rw_emulate(m_channel, data, len);
        }
        return true;
    }

    virtual const std::string printConfig() override{
        std::string result = "SPI\n";
        result += " Channels: " + std::to_string(m_channels);
        for(int i = 0; i < m_channels; i++){
            result += " Channel: " + std::to_string(i+1) = " Speed: " + std::to_string(m_speed[i]) + " Mode: " + std::to_string(m_mode[i]) + "\n";
        }
        return result;
    }

private:
    std::recursive_mutex spi_mtx;

    int _fd[2];
    SPI_MODE m_mode[2]; //SPI bus mode
    int m_speed[2];     //speed
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio[2];

    int m_channel;      //current active channel
    int m_channels;     //number of channels
    bool m_real_world;

private:
    /*
    *
    */
    unsigned short m_test_value = 0;
    void data_rw_emulate(int channel, unsigned char* data, int len){
        /*
        *Emulate some data from hardware level
        */
        if(len == 3){ //Analog Light. Construct 12-bit value
            unsigned short pin = ((data[0] & 0x01) << 2) | (data[1] >> 6);

            data[2] = (unsigned char)(m_test_value & 0x00FF);
            data[1] |= (unsigned char)((m_test_value >> 8) & 0x0F);

            if(m_test_value % (144*(pin+1)) != 0)
                m_test_value += 2;
            else
                m_test_value += 16;

            if(m_test_value>= 4096)
                m_test_value = 0;
        }
    }
};

} //namespace spi
} /* namespace pirobot */

#endif
