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
#include <linux/spi/spidev.h>

#include "logger.h"
#include "provider.h"
#include "gpio.h"

namespace pirobot {
namespace spi {

/*
Several ioctl() requests let your driver read or override the device's current
settings for data transfer parameters:

    SPI_IOC_RD_MODE, SPI_IOC_WR_MODE ... pass a pointer to a byte which will
	return (RD) or assign (WR) the SPI transfer mode.  Use the constants
	SPI_MODE_0..SPI_MODE_3; or if you prefer you can combine SPI_CPOL
	(clock polarity, idle high iff this is set) or SPI_CPHA (clock phase,
	sample on trailing edge iff this is set) flags.
	Note that this request is limited to SPI mode flags that fit in a
	single byte.

    SPI_IOC_RD_MODE32, SPI_IOC_WR_MODE32 ... pass a pointer to a uin32_t
	which will return (RD) or assign (WR) the full SPI transfer mode,
	not limited to the bits that fit in one byte.

    SPI_IOC_RD_LSB_FIRST, SPI_IOC_WR_LSB_FIRST ... pass a pointer to a byte
	which will return (RD) or assign (WR) the bit justification used to
	transfer SPI words.  Zero indicates MSB-first; other values indicate
	the less common LSB-first encoding.  In both cases the specified value
	is right-justified in each word, so that unused (TX) or undefined (RX)
	bits are in the MSBs.

    SPI_IOC_RD_BITS_PER_WORD, SPI_IOC_WR_BITS_PER_WORD ... pass a pointer to
	a byte which will return (RD) or assign (WR) the number of bits in
	each SPI transfer word.  The value zero signifies eight bits.

    SPI_IOC_RD_MAX_SPEED_HZ, SPI_IOC_WR_MAX_SPEED_HZ ... pass a pointer to a
	u32 which will return (RD) or assign (WR) the maximum SPI transfer
	speed, in Hz.  The controller can't necessarily assign that specific
	clock speed.
*/

/*
#define SPI_CPHA        0x01
#define SPI_CPOL        0x02

#define SPI_MODE_0      (0|0)
#define SPI_MODE_1      (0|SPI_CPHA)
#define SPI_MODE_2      (SPI_CPOL|0)
#define SPI_MODE_3      (SPI_CPOL|SPI_CPHA)

#define SPI_CS_HIGH     0x04
#define SPI_LSB_FIRST       0x08
#define SPI_3WIRE       0x10
#define SPI_LOOP        0x20
#define SPI_NO_CS       0x40
#define SPI_READY       0x80
*/

enum SPI_MODE{
    MODE_0 = SPI_MODE_0,
    MODE_1 = SPI_MODE_1,
    MODE_2 = SPI_MODE_2,
    MODE_3 = SPI_MODE_3,
    MODE_UNKNOWN = -1
};

enum SPI_CHANNELS {
    SPI_0 = 0,
    SPI_1 = 1,
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
        std::shared_ptr<pirobot::gpio::Gpio> ce0, std::shared_ptr<pirobot::gpio::Gpio> ce1) noexcept(false) :
        Provider(pirobot::provider::PROVIDER_TYPE::PROV_SPI, name),
        m_channel(SPI_CHANNELS::SPI_0), m_channels(config.channels), m_real_world(config.real_world)
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
                _fd[i] = setup((i==0 ? SPI_CHANNELS::SPI_0 : SPI_CHANNELS::SPI_1), config.speed[i], config.mode[i]);
        }

        //switch OFF by default
        Off();
    }

    /*
    * Initialize SPI channel
    */
    int setup(const SPI_CHANNELS channel, const int speed, const SPI_MODE mode){
        logger::log(logger::LLOG::INFO, "SPI", std::string(__func__) + std::string(" Initializing channel: ") + std::to_string(channel));

        int fd = open(spi_dev_name[channel], O_RDWR);
        if( fd < 0){
            logger::log(logger::LLOG::ERROR, "SPI", std::string(__func__) + (" Could not open SPI channel errno: ") + std::to_string(errno));
            return -1;
        }

        bool success = true;
        if(ioctl (fd, SPI_IOC_WR_MODE, &mode) < 0){
            logger::log(logger::LLOG::ERROR, "SPI", std::string(__func__) + (" Could not set SPI mode errno: ") + std::to_string(errno));
            success = false;
        }
        else{
            if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &_bits_per_word) < 0){
                logger::log(logger::LLOG::ERROR, "SPI", std::string(__func__) + (" Could not set SPI bits per world errno: ") + std::to_string(errno));
                success = false;
            }
            else{
                if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0){
                    logger::log(logger::LLOG::ERROR, "SPI", std::string(__func__) + (" Could not set SPI speed errno: ") + std::to_string(errno));
                    success = false;
                }
            }
        }

        if(!success){
            close(fd);
            fd = -1;
        }
        return fd;
    }

    /*
    * Get mode for SPI channel
    */
    SPI_MODE get_mode(const SPI_CHANNELS channel) const {
        uint8_t mode;
        if(ioctl (_fd[channel], SPI_IOC_RD_MODE, &mode) < 0){
            logger::log(logger::LLOG::ERROR, "SPI", std::string(__func__) + (" Could not get SPI mode errno: ") + std::to_string(errno));
            return SPI_MODE::MODE_UNKNOWN;
        }

        if(mode!=SPI_MODE::MODE_0 && mode!=SPI_MODE::MODE_1 && mode!=SPI_MODE::MODE_2 && mode!=SPI_MODE::MODE_3)
            return SPI_MODE::MODE_UNKNOWN;

        return (SPI_MODE)mode;
    }

    /*
    * Get maximum speed for SPI channel
    */
    uint32_t get_speed(const SPI_CHANNELS channel) const {
        uint32_t speed;
        if(ioctl (_fd[channel], SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0){
            logger::log(logger::LLOG::ERROR, "SPI", std::string(__func__) + (" Could not get SPI speed errno: ") + std::to_string(errno));
            return 0;
        }
        return speed;
    }


    //
    virtual ~SPI(){
        //Close SPI channels
        for(int i = 0; i < m_channels; i++){
            if(_fd[i] > 0)
                close(_fd[i]);
        }
    }

    /*
    * Off both channels using accordingly GPIO
    */
    inline void Off(){
        for(int i = 0; i < m_channels; i++)
            set_channel_off(i==0 ? SPI_CHANNELS::SPI_0 : SPI_CHANNELS::SPI_1);
    }

    /*
    * Switch On SPI channel using accordingly GPIO
    */
    bool set_channel_on(const SPI_CHANNELS channel){
        if(channel >= m_channels){
            logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " Invalid channel number: " + std::to_string(channel));
            return false;
        }

        //do not need to do anything - there is current channel
        if(channel == m_channel){
            return true;
        }

        //if another channel is active
        if(m_channel >= 0){
            set_channel_off(channel);
        }

        //switch GPIO for channel to LOW
        if(!m_gpio[channel]){
            logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " No GPIO for channel: " + std::to_string(channel));
            return false;
       }

        logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " ON Done. Channel: " + std::to_string(channel));

        std::lock_guard<std::mutex> lock(spi_mtx);
        m_channel = channel;
        m_gpio[channel]->Low();

        return true;
    }

    /*
    * Switch Off SPI channel using accordigly GPIO
    */
    bool set_channel_off(const SPI_CHANNELS channel) noexcept(false){
        logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " OFF Channel: " + std::to_string(channel));

        if(channel >= m_channels){
            logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " Incorrect channel: " + std::to_string(channel));
            return false;
        }

        //set GPIO for this channel to HIGH
        if(!m_gpio[channel]){
            logger::log(logger::LLOG::DEBUG, "SPI", std::string(__func__) + " No GPIO for channel: " + std::to_string(channel));
            return false;
        }

        if(m_gpio[channel]->is_High()){
            return true; //already High
        }

        std::lock_guard<std::mutex> lock(spi_mtx);
        m_gpio[channel]->High();
        return true;
    }

    /*
    * Read and Write data:
    */
    int data_read_write(const SPI_CHANNELS channel, unsigned char* data, int len){
        struct spi_ioc_transfer spi;
        memset (&spi, 0, sizeof (spi)) ;

        spi.tx_buf        = (uint64_t)data ;
        spi.rx_buf        = (uint64_t)data ;
        spi.len           = len ;
        spi.delay_usecs   = _delay ;
        spi.speed_hz      = m_speed[channel] ;
        spi.bits_per_word = _bits_per_word ;

        int result = ioctl (_fd[channel], SPI_IOC_MESSAGE(1), &spi);
        if(result < 0){
            logger::log(logger::LLOG::ERROR, "SPI", std::string(__func__) + (" RW SPI failed errno: ") + std::to_string(errno));
        }

        return result;
    }

    /*
    * Read data from device over SPI bus
    */
    bool data_rw(unsigned char* data, int len){
        int ret = 0;
        if(m_real_world){
            ret = data_read_write(m_channel, data, len);
        }
        else{
            ret = data_rw_emulate(m_channel, data, len);
        }
        return (ret>=0);
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
    std::mutex spi_mtx;

    int _fd[2];
    SPI_MODE m_mode[2]; //SPI bus mode
    int m_speed[2];     //speed
    std::shared_ptr<pirobot::gpio::Gpio> m_gpio[2];

    SPI_CHANNELS m_channel;      //current active channel
    int m_channels;     //number of channels
    bool m_real_world;

    const char* spi_dev_name[2] = {"/dev/spidev0.0", "/dev/spidev0.1"};
    uint16_t _delay = 0;
    uint8_t  _bits_per_word = 8;

    /*
    *
    */
    unsigned short m_test_value = 0;
    int data_rw_emulate(const SPI_CHANNELS channel, unsigned char* data, int len){
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
        return 0;
    }
};

} //namespace spi
} /* namespace pirobot */

#endif
