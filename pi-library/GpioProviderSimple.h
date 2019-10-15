/*
 * GpioProviderSimple.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_GPIOPROVIDERSIMPLE_H_
#define PI_LIBRARY_GPIOPROVIDERSIMPLE_H_

#include <cstdlib>
#include <poll.h>
#include <sys/ioctl.h>

#include "GpioProvider.h"
#include "smallthings.h"
#include "timers.h"
#include "Threaded.h"

namespace pirobot {
namespace gpio {

/*
* BCM2835 ARM peripherials (section 6.1 Register View)
*/
using gpio_ctrl = struct __attribute__((packed, aligned(4))) gpio_ctrl {
    uint32_t _GPFSEL[6];  //GPIO Function Select 0,1,2,3,4,5
    uint32_t _reserved1;

    uint32_t _GPSET[2];   //GPIO Pin Output Set 0,1 (W)
    uint32_t _reserved2;

    uint32_t _GPCLR[2];   //GPIO Pin Output Clear 0,1 (W)
    uint32_t _reserved3;

    uint32_t _GPLEV[2];   //GPIO Pin Level 0,1 (R)
    uint32_t _reserved4;

    uint32_t _GPEDS[2];   //GPIO Pin Event Detect Status 0,1
    uint32_t _reserved5;

    uint32_t _GPREN[2];   //GPIO Pin Rising Edge Detect Enable 0,1
    uint32_t _reserved6;

    uint32_t _GPFEN[2];   //GPIO Pin Falling Edge Detect Enable 0,1
    uint32_t _reserved7;

    uint32_t _GPHEN[2];   //GPIO Pin High Detect Enable 0,1
    uint32_t _reserved8;

    uint32_t _GPLEN[2];   //GPIO Pin Low Detect Enable 0,1
    uint32_t _reserved9;

    uint32_t _GPAREN[2];  //GPIO Pin Async. Rising Edge Detect 0,1
    uint32_t _reserved10;

    uint32_t _GPAFEN[2];  //GPIO Pin Async. Falling Edge Detect 0,1
    uint32_t _reserved11;

    uint32_t _GPPUD;    //GPIO Pin Pull-up/down Enable

    uint32_t _GPPUDCLK[2];// GPIO Pin Pull-up/down Enable Clock 0,1
    uint32_t _reserved12;
};

class Gpio;

class GpioProviderSimple : public GpioProvider, public piutils::Threaded
{
public:
    GpioProviderSimple(const std::string name = "SIMPLE", const int pins = s_pins) noexcept(false);
    virtual ~GpioProviderSimple();

    virtual const int dgtRead(const int pin) override;
    virtual void dgtWrite(const int pin, const int value) override;
    virtual void setmode(const int pin, const GPIO_MODE mode) override;
    virtual void pullUpDownControl(const int pin, const PULL_MODE pumode) override;
    virtual void setPulse(const int pin, const uint16_t pulselen) override;

    virtual const GPIO_MODE getmode(const int pin) override;

    virtual const GPIO_PROVIDER_TYPE get_type() const override { return GPIO_PROVIDER_TYPE::PROV_SIMPLE; }
    virtual const std::string to_string() override;

    virtual const std::string printConfig() override {
        return to_string() + "\n";
    }

    virtual bool is_support_group() override {
        return true;
    }

    void start();

    static const int s_pins = 26;

    //set edge and level detection for pin (if supported)
    virtual bool set_egde_level(const int pin, GPIO_EDGE_LEVEL edgs_level) override {
        logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + std::string(" pin: ") + std::to_string(pin) + " Edge&Level: " + std::to_string(edgs_level));

        std::cout << "set_egde_level " << std::endl;

        if( !is_support_group()){
            std::cout << "set_egde_level not supported" << std::endl;
            return false;
        }

        /*
        * Switch GPIO detection off
        * We will not export GPIO each time
        */
        if(edgs_level == GPIO_EDGE_LEVEL::EDGE_NONE){
            std::cout << "set_egde_level edge OFF" << std::endl;

            //check if GPIO was exported - if TRUE - change value in /edge folder
            if(is_gpio_exported(pin)){
                close_gpio_folder(pin);

                return _set_gpio_edge(pin, edgs_level);
            }

            return true;
        }

        if(is_gpio_exported(pin)){
            close_gpio_folder(pin);
        }
        else if(!export_gpio(pin)){ 
            std::cout << "open_gpio_folder EXPORT GPIO failed " << std::endl;
            return false;
        }

        if(!_set_gpio_edge(pin, edgs_level)){
            return false;
        }

        if(!open_gpio_folder(pin)){
            std::cout << "set_egde_level Open folder failed" << std::endl;
            return false;
        }

        return true;
    }

    /*
    * Print status for all Edge related registers
    */
    const std::string print_edge_status() {
        std::string result = 
            "Rising  Edge GPREN1: " + std::to_string(_gctrl->_GPREN[0]) +
            "\nFalling Edge GPFEN1: " + std::to_string(_gctrl->_GPFEN[0]) + 
            "\nHigh Detect  GPHEN1: " + std::to_string(_gctrl->_GPHEN[0]) + 
            "\nLow  Detect  GPLEN1: " + std::to_string(_gctrl->_GPLEN[0]) + "\n";

        return result;
    }

    /*
    * Print mode for each GPIO PIN
    */
    const std::string print_mode() {
      std::string result;

      for(int i = 0; i < pins(); i++){
           result +=  "Pin: " + std::to_string(i) + " [" + std::to_string(phys_pin(i)) + "] Mode: " + std::to_string(getmode(i)) + "\n";
      }
      return result;
    }

    friend class Gpio;

    /*
    *
    */
    static void worker(GpioProviderSimple* owner){
        logger::log(logger::LLOG::DEBUG, "PrvSmpl", std::string(__func__) + "Worker started.");

        struct pollfd* pfd = owner->get_fds();
        nfds_t nfd = GpioProviderSimple::s_pins;
        int fd_timeout = 950;
        char rbuff[5];
        struct pollfd loc_pfd[5];

        //check if we ready to start - we has GPIO for test
        auto fn = [owner]{return (owner->is_stop_signal() | owner->fd_count() > 0);};

        while(!owner->is_stop_signal()){

            {
                std::cout << "wait for signal" << std::endl;
                std::unique_lock<std::mutex> lk(owner->cv_m);
                owner->cv.wait(lk, fn);

                //logger::log(logger::LLOG::DEBUG, "PrvSmpl", std::string(__func__) + std::string(" FD count: ") + std::to_string(owner->fd_count()));
            }


            volatile int fd_cnt = owner->fd_count();
            bool is_stop = owner->is_stop_signal();
            std::cout << "signal detected FD count: " << fd_cnt << " is stop : " << is_stop << std::endl;

            if(owner->is_stop_signal()){
                 std::cout << "STOP signal detected" << std::endl;
                 break;
            }


            while(1){

                if(fd_cnt == 0 || owner->is_stop_signal()){
                    break;
                }
                else {
                    std::lock_guard<std::mutex> lock(owner->get_mutex());
                    //initialize data
                    int cnt = 0;
                    for(int i=0; i<nfd && cnt<fd_cnt; i++){
                        if(pfd[i].fd < 0 )
                        continue;
                        loc_pfd[cnt].fd = pfd[i].fd;
                        loc_pfd[cnt].events = POLLPRI;
                        loc_pfd[cnt].revents = 0;
                        cnt++;
                    }
                }

                //sleep(2);
                //make request
                std::cout << "before poll signal detected" << std::endl;
                int res = poll(loc_pfd, fd_cnt, fd_timeout);

                std::cout << "after poll signal detected " << res <<std::endl;
                if(res == 0){ //nothing happened or timeout
                    continue;
                }
                else if(res < 0){ //error
                    logger::log(logger::LLOG::ERROR, "PrvSmpl", std::string(__func__) + " Poll error: " + std::to_string(errno));
                    //TODO: what next?
                }
                else{ //we have an update for some descriptors
                    //process events one by one

                    std::cout << "start check descriptors " << std::endl;
                    for(int i = 0; i < fd_cnt && res > 0; i++ ){
                        //we have something for this descriptor
                        if(loc_pfd[i].fd > 0 && loc_pfd[i].revents != 0){

                            std::cout << "descriptor " << i << " revents: " << loc_pfd[i].revents << std::endl;

                            if((loc_pfd[i].revents&POLLPRI) != 0){ //get a new value and notify
                                memset(rbuff, 0x00, 5);
                                lseek(loc_pfd[i].fd, 0, SEEK_SET);
                                int rres = read(loc_pfd[i].fd, rbuff, 4);
                                if(rres < 0){
                                    logger::log(logger::LLOG::ERROR, "PrvSmpl", std::string(__func__) + " Poll Pin: " + std::to_string(i) + " Read error: " + std::to_string(errno));
                                }

                                printf("Value %x:%x:%x:%x\n", rbuff[0], rbuff[1], rbuff[2], rbuff[3]);
                                std::cout << "descriptor counter " << res << " read: " << rres << std::endl;

                                res--; //not need to check if we have already processed all

                            }
                            else if((loc_pfd[i].revents&POLLERR) != 0){ //something wrong here
                                logger::log(logger::LLOG::ERROR, "PrvSmpl", std::string(__func__) + " Poll Pin: " + std::to_string(i) + " Revents: " + std::to_string(loc_pfd[i].revents));
                            }
                        }
                    }

                    std::cout << "stop check descriptors " << std::endl;
                }
            }
        }

        std::cout << "--- worker finished ----" <<  std::endl;
        logger::log(logger::LLOG::DEBUG, "PrvSmpl", std::string(__func__) + "Worker finished.");
    }

    void stop(){
        logger::log(logger::LLOG::DEBUG, "PrvSmpl", std::string(__func__));

        std::cout << "--- stop ----" <<  std::endl;
        piutils::Threaded::stop(true);
        std::cout << "--- stop finished ----" <<  std::endl;
    }


protected:

    struct pollfd* get_fds() {
        return _fds;
    }

    const int fd_count() {
        return _fd_count;
    }

    std::mutex& get_mutex() {
        return _mx_gpio;
    }

    int _fd_count = 0;

    /*
    * Some GPIO providers supoprt level detection through interrupt
    * It can be useful for detection multiple states changing (usually IN; for example buttons) instead of polling
    */
    static const size_t s_buff_size = 32;

    bool export_gpio(const int pin) {
        logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + std::string(" pin: ") + std::to_string(pin));

        std::cout << "export_gpio" << std::endl;

        int phpin = phys_pin(pin);
        auto pin_dir = get_gpio_path(phpin);
        if(!piutils::chkfile(pin_dir)){

            std::cout << "export_gpio no folder " << pin_dir << std::endl;

            logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + " No folder for pin: " + pin_dir);
            std::string command = std::string("echo ") + std::to_string(phpin) + std::string(" > /sys/class/gpio/export");

            int res = std::system(command.c_str());
            logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + " Executed command: " + command + " result: " + std::to_string(res));

            std::cout << "export_gpio command res: " << res << std::endl;

            //check result
            if(!piutils::chkfile(pin_dir)){
                //logger::log(logger::LLOG::ERROR, "PrvSmpl", std::string(__func__) + std::string(" Could not create folder: ") + pin_dir);
                std::cout << "export_gpio check NO folder " << std::endl;
                return false;
            }

            std::cout << "export_gpio success" << std::endl;
        }
        else{
            std::cout << "export_gpio already exported" << std::endl;
        }

        return true;
    }

    /*
    *
    */
    bool unexport_gpio(const int pin) {
        logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + std::string(" pin: ") + std::to_string(pin));

        int phpin = phys_pin(pin);
        auto pin_dir = get_gpio_path(phpin);
        if(piutils::chkfile(pin_dir)){
            logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + " Folder exist: " + pin_dir);
            std::string command = std::string("echo ") + std::to_string(phpin) + std::string(" > /sys/class/gpio/unexport");

            int res = std::system(command.c_str());
            logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + " Executed command: " + command + " result: " + std::to_string(res));

            piutils::timers::Timers::delay(500);

            if(piutils::chkfile(pin_dir)){
                logger::log(logger::LLOG::ERROR, "PrvSmpl", std::string(__func__) + std::string(" Could not delete folder: ") + pin_dir);
                return false;
            }
        }

        return true;
    }

    /*
    * Is GPIO exported
    */
   bool is_gpio_exported(const int pin){
        int phpin = phys_pin(pin);
        auto pin_dir = get_gpio_path(phpin);
        return piutils::chkfile(pin_dir);
   }

    /*
    *
    */
    const std::string get_edge_name(const GPIO_EDGE_LEVEL edgs_level) const{

        if(edgs_level == GPIO_EDGE_LEVEL::EDGE_RAISING) return std::string("rising");
        if(edgs_level == GPIO_EDGE_LEVEL::EDGE_FALLING) return std::string("falling");
        if(edgs_level == GPIO_EDGE_LEVEL::EDGE_BOTH) return std::string("both");

        return std::string("none");
    }

    /*
    *
    */
   bool _set_gpio_edge(const int pin, GPIO_EDGE_LEVEL edge_level){
       
        const std::string edge = get_edge_name(edge_level);
        int phpin = phys_pin(pin);
        auto pin_dir = get_gpio_path(phpin, std::string("edge"));

        std::string command = std::string("echo ") +edge + std::string(" > ") + pin_dir;
        int res = std::system(command.c_str());

        logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + " Executed command: " + command + " result: " + std::to_string(res));
        return true;
   }

    /*
    *
    */
    bool open_gpio_folder(const int pin){

        std::cout << "open_gpio_folder EXPORTed GPIO " << pin << " FD: " << _fds[pin].fd << std::endl;

        if( _fds[pin].fd < 0){ //if file for this GPIO is not open yet - do it

            const auto pin_dir = get_gpio_path(phys_pin(pin), std::string("value"));
            {
                std::lock_guard<std::mutex> lock(_mx_gpio);
                _fds[pin].fd = open(pin_dir.c_str(), O_RDONLY);
                
                if( _fds[pin].fd > 0){
                    int cnt = 0;
                    int res = ioctl(_fds[pin].fd, FIONREAD , &cnt);
                    if(res >= 0){ //read all older values
                        char ch;
                        for(int i=0; i<cnt; i++){
                            ssize_t rret = read(_fds[pin].fd, &ch, 1);
                            if(rret < 0) break;
                        }
                    }
                    _fd_count++;
                    cv.notify_one();
                }
            }

            std::cout << "open_gpio_folder opened GPIO " << pin << " FD: " << _fds[pin].fd << " File: " << pin_dir.c_str() << std::endl;
            if( _fds[pin].fd < 0){
                logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + std::string(" Could not open: ") + pin_dir + " Error:" + std::to_string(errno));
                return false;
            }
        }

        std::cout << "open_gpio_folder success " << std::endl;
        return true;
    }

    /*
    *
    */
    void close_gpio_folder(const int pin, const bool unexport = false){

        if( _fds[pin].fd > 0){
            logger::log(logger::LLOG::INFO, "PrvSmpl", std::string(__func__) + std::string(" Switch OFF detection for pin: ") + std::to_string(pin));
            {
                std::lock_guard<std::mutex> lock(_mx_gpio);
                close(_fds[pin].fd);
                _fds[pin].fd = -1;

                _fd_count--;
           }
           //cv.notify_one();
        }

        if(unexport){ //unexport GPIO
            unexport_gpio(pin);
        }
    }

private:
    volatile gpio_ctrl* _gctrl; //pointer to GPIO registers map
    std::mutex _mx_gpio;

    //poll of GPIO descriptors
    /*
    struct pollfd {
               int   fd;         // file descriptor
               short events;     // requested events
               short revents;    // returned events
           };
    */
    struct pollfd _fds[s_pins];

    const int phys_pin(const int pin) const {
        return pins_map[getRealPin(pin)];
    }

    //Prepare directory name for physical PIN number
    const std::string get_gpio_path(const int phpin, const std::string folder = ""){
        char buff[s_buff_size];
        if(folder.length() > 0)
            snprintf(buff, s_buff_size, "/sys/class/gpio/gpio%d/%s", phpin, folder.c_str());
        else
            snprintf(buff, s_buff_size, "/sys/class/gpio/gpio%d", phpin);

        std::string pin_dir = buff;

        return pin_dir;
    }

    /*
        GPIO
        General     0-6   GPIO [17, 18, 27, 22, 23, 24, 25]
        Additional  7-13  GPIO [ 5, 6,  19, 16, 26, 20, 21]
        PWM         14,15 GPIO [12, 13]
        I2C         16,17 GPIO [ 2, 3]
        SPI         18-22 GPIO [ 7, 8, 9, 10, 11] CE1_N, CE0_N, SO, SI, SCLK
        GCLK        23         [4]
        UART TX/RX  24,25      [14, 15]
    */
    const int pins_map[26] = {
        17, 18, 27, 22, 23, 24, 25, 5, 6, 19, 16, 26, 20, 21,
        12, 13,
        2,  3,
        7,  8,  9, 10, 11,
        4,
        14, 15
    };

    using pin_mode = std::tuple<int, GPIO_MODE>;

    const pin_mode pins_pwm[4] = {
        std::make_tuple(12, GPIO_MODE::ALT0),
        std::make_tuple(18, GPIO_MODE::ALT5),
        std::make_tuple(13, GPIO_MODE::ALT0),
        std::make_tuple(18, GPIO_MODE::ALT5)
    };

    GPIO_MODE get_pwm_mode(const int pin) const {
        for(int i = 0; i < 4; i++){
            if(std::get<0>(pins_pwm[i]) == pin)
                return std::get<1>(pins_pwm[i]);
        }

        return GPIO_MODE::PWM_OUT;
    }

};

}
}
#endif /* PI_LIBRARY_GPIOPROVIDERSIMPLE_H_ */
