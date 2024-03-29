/*
 * core_pwm.h
 *
 * BCM2835 ARM Peripherals. PWM
 *
 *  Created on: Jan 15, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_CORE_PWM_H_
#define PI_LIBRARY_CORE_PWM_H_

#include "smallthings.h"
#include "logger.h"


#include "core_memory.h"
#include "core_dma.h"
#include "core_clock_pwm.h"

namespace pi_core {
namespace core_pwm {

#define DEV_MEM     "/dev/mem"

#define PWM_OFFSET                               (0x0020c000)   //use peripheral address as base for this address
#define PWM_PERIPH_PHYS                          (0x7e20c000)   //use this addrewss directly

using pwm_regs = struct __attribute__((packed, aligned(4))) pwm_regs_t{
    uint32_t _ctl;  //PWM Control
    uint32_t _sta;  //PWM Status
    uint32_t _dmac; //PWM DMA configuration
    uint32_t _resrvd1;
    uint32_t _rng1; //PWM channel 1 range
    uint32_t _dat1; //PWM channel 1 data
    uint32_t _fif1; //PWM FIFO input. This register is the FIFO input for the all channels.
    uint32_t _resrvd2;
    uint32_t _rng2; //PWM channel 2 range
    uint32_t _dat2; //PWM channel 2 data
};


/*
* PWM Control register (CTL)
*/
//bits 31-16 not used, write 0
#define PWM_CTRL_MSEN2 (1<<15)  // RW bit 15; Channel 1 M/S Enable; (0-PWM algorithm used; 1-M/S transmission used)
//bit 14-not used, write 0
#define PWM_CTRL_USEF2  (1<<13) // RW bit 13; Channel 2 use FIFO (0-Data register is transmitted; 1-FIFO used for transmission)
#define PWM_CTRL_POLA2  (1<<12) // RW bit 12; Channel 2 polarity; (0-[0:low; 1-high], 1-[1-low; 0-high])
#define PWM_CTRL_SBIT2  (1<<11) // RW bit 11; Channel 2 silence bit; Defines the state of the output when no transmission takes place
#define PWM_CTRL_RPTL2  (1<<10) // RW bit 10; Channel 2 repeat last data; (0-Transmission interrupts when FIFO is empty,
                                //                                        1-Last data in FIFO is transmitted repetedly until FIFO is not empty)
#define PWM_CTRL_MODE2  (1<<9)  // RW bit 9; Channel 2 mode; (0-PWM mode, 1-Serialiser mode)
#define PWM_CTRL_PWEN2  (1<<8)  // RW bit 8; Channel 2 enable (0-disable, 1-enable)

#define PWM_CTRL_MSEN1  (1<<7)  // RW bit 7; Channel 1 M/S Enable; (0-PWM algorithm used; 1-M/S transmission used)
#define PWM_CTRL_CLRF1  (1<<6)  // RO bit 6; (1-Clears FIFO, 0-Has no effect; This is single shot operation)
                                // This bit always reads 0
#define PWM_CTRL_USEF1  (1<<5)  // RW bit 5; Channel 1 use FIFO (0-Data register is transmitted; 1-FIFO used for transmission)
#define PWM_CTRL_POLA1  (1<<4)  // RW bit 4; Channel 1 polarity; (0-[0:low; 1-high], 1-[1-low; 0-high])
#define PWM_CTRL_SBIT1  (1<<3)  // RW bit 3; Channel 1 silence bit; Defines the state of the output when no transmission takes place
#define PWM_CTRL_RPTL1  (1<<2)  // RW bit 2; Channel 1 repeat last data; (0-Transmission interrupts when FIFO is empty,
                                //                                        1-Last data in FIFO is transmitted repetedly until FIFO is not empty)
#define PWM_CTRL_MODE1  (1<<1)  // RW bit 1; Channel 1 mode; (0-PWM mode, 1-Serialiser mode)
#define PWM_CTRL_PWEN1  (1<<0)  // RW bit 0; Channel 1 enable (0-disable, 1-enable)

/*
* PWM Status register (STA)
*/
#define PWM_STA_STA4    (1<<12)   // RW bit 12; Channel 4 State
#define PWM_STA_STA3    (1<<11)   // RW bit 11; Channel 3 State
#define PWM_STA_STA2    (1<<10)   // RW bit 10; Channel 2 State
#define PWM_STA_STA1    (1<<9)   // RW bit  9; Channel 1 State
#define PWM_STA_BERR    (1<<8)   // RW bit 8; Bus error flag
#define PWM_STA_GAPO4   (1<<7)   // RW bit 7; Channel 4 Gap occurred flag
#define PWM_STA_GAPO3   (1<<6)   // RW bit 6; Channel 3 Gap occurred flag
#define PWM_STA_GAPO2   (1<<5)   // RW bit 5; Channel 2 Gap occurred flag
#define PWM_STA_GAPO1   (1<<4)   // RW bit 4; Channel 1 Gap occurred flag
#define PWM_STA_RERR1   (1<<3)   // RW bit 3; FIFO Read error flag
#define PWM_STA_WERR1   (1<<2)   // RW bit 2; FIFO Write error flag
#define PWM_STA_EMPT1   (1<<1)   // RW bit 1; FIFO Empty flag
#define PWM_STA_FULL1   (1<<0)   // RW bit 0; FIFO full flag

/*
* PWM DMA Configuration register (DMAC)
*/
#define RPI_PWM_DMAC_ENAB        (1<<31)                     //RW bit  31; DMA Enable; (0-DMA disabled; 1-DMA enabled)
#define RPI_PWM_DMAC_PANIC_RESET RPI_PWM_DMAC_PANIC_RESET(7) //RW bits 7-0; DMA Treshold for PANIC signal (reset value 0x7)
#define RPI_PWM_DMAC_PANIC(val)  ((val & 0xff) << 8)    //RW bits 15-8; DMA Treshold for PANIC signal (reset value 0x7)
#define RPI_PWM_DMAC_DREQ_RESET  RPI_PWM_DMAC_DREQ(7)   //RW bits 7-0; DMA Treshold for DREQ signal (reset value 0x7)
#define RPI_PWM_DMAC_DREQ(val)   (val & 0xff)           //RW bits 7-0; DMA Treshold for DREQ signal (reset value 0x7)

/*
* PWM Channel 1 Range register (RNG1)
*/
#define RPI_PWM_RNG1_RESET  0x20

/*
* PWM Channel 1 Data register (DAT1)
*/
#define RPI_PWM_DAT1_RESET  0x0


/*
* PWM Channel 2 Range register (RNG2)
*/
#define RPI_PWM_RNG2_RESET  0x20

/*
* PWM Channel 2 Data register (DAT2)
*/
#define RPI_PWM_DAT2_RESET  0x0


class PwmCore : public pi_core::core_mem::PhysMemory
{
public:
    PwmCore(const uint16_t dma = 10) : pi_core::core_mem::PhysMemory(),
        _pwm_regs(nullptr), _pwm_clock(nullptr), _dma_ctrl(nullptr)
    {
        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__));

        _pwm_clock = new pi_core::core_clock_pwm::PwmClock();

        /*
        * Do not use DMA for PWM
        */
        if(dma > 0){
            _dma_ctrl = new pi_core::core_dma::DmaControl(dma);
        }

        uint32_t ph_address = pi_core::CoreCommon::get_peripheral_address();

        _pwm_regs = piutils::map_memory<pwm_regs>(ph_address + pwmctl_addr);
        if( _pwm_regs == nullptr){
            logger::log(logger::LLOG::ERROR, "PwmCore", std::string(__func__) + " Fail to initialize PWM control");
            throw std::runtime_error(std::string("Fail to initialize PWM control"));
        }
    }

    virtual ~PwmCore(){
        logger::log(logger::LLOG::INFO, "PwmCore", std::string(__func__));

        //std::cout << " ~PwmCore Clock" << std::endl;
        if(_is_clock()){
            delete _pwm_clock;
        }

        //std::cout << " ~PwmCore DMA" << std::endl;
        if(_is_dma()){
            delete _dma_ctrl;
        }

        if(_is_init()){
            //std::cout << " ~PwmCore PWM" << std::endl;
            _clear();
            _pwm_regs = piutils::unmap_memory<pwm_regs>((pwm_regs*)_pwm_regs);
        }
    }

    /*
    * Write data to PWM
    */
    virtual bool write_data(std::uintptr_t data, int len){
        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__) + " Write to PWM: " +  std::to_string(len));

        if(!_is_dma()){
            logger::log(logger::LLOG::INFO, "PwmCore", std::string(__func__) + " No DMA");
            return false;
        }

        /*
        * TODO: Make it Asynchroniously
        */

        pi_core::core_dma::DmaControlBlock* cb = new pi_core::core_dma::DmaControlBlock(this);
        bool result = cb->prepare(data, this->get_fifo_address(), len);
        if( result ){
            logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__) + " Start to process DMA Control Block");
            _dma_ctrl->print_status();
            result = _dma_ctrl->process_control_block(cb);
            if( result ){
                int i = 0;
                while( !_dma_ctrl->is_finished() && ++i < 10){
                  std::this_thread::sleep_for(std::chrono::seconds(1));
                  _dma_ctrl->print_status();
                  //std::this_thread::sleep_for(std::chrono::microseconds(10));
                }

                if(i>= 10){
                  //std::cout << " ******* FAILED ************" << std::endl;
                  _dma_ctrl->print_status();

                  result = false;
                }
                else
                  _dma_ctrl->print_status();

                _dma_ctrl->reset();
                //TODO: Check finish state?
            }
            else
              logger::log(logger::LLOG::ERROR, "PwmCore", std::string(__func__) + " DMA Control Block processing failed");

            logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__) + " DMA Control Block processed");
        }
        else
            logger::log(logger::LLOG::ERROR, "PwmCore", std::string(__func__) + " DMA Ctrl prepare failed");

        delete cb;

        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__) + " Finished");
        return result;
    }

    const uint32_t pwmctl_addr = PWM_OFFSET;
    const uint32_t pwmctl_addr_phys = PWM_PERIPH_PHYS;

    const uint32_t pwmctl_frequency = 19200000;

    /*
    * Initialize connection
    */
    bool Initialize() {

       _stop();

        if( _is_clock() ){
            if( !_pwm_clock->Initialize()){
                logger::log(logger::LLOG::ERROR, "PwmCore", std::string(__func__) + " Fail to initialize PWM Clock");
                return false;
            }
        }

        if( _is_dma() ){
            if( !_dma_ctrl->Initialize()){
                logger::log(logger::LLOG::ERROR, "PwmCore", std::string(__func__) + " Fail to initialize DMA Control");
                return false;
            }
        }

        //Set PWM configuration
        _configure();

        return true;
    }

    /*
    * Get physical address of FIFO register
    */
   const uintptr_t get_fifo_address() const{
        if(_is_init())
            return (uintptr_t)&((pwm_regs*)PWM_PERIPH_PHYS)->_fif1;

        return (uintptr_t)nullptr;
   }

private:

    /*
    * Check if low level registers access was succwssfully configured
    */
    const bool _is_init() const{
        return (_pwm_regs != nullptr);
    }

    /*
    * Set initial values for PWM
    */
   void _configure() {
        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__));

        ////std::cout << "C1 PWM STA  0x" << std::hex <<_pwm_regs->_sta << std::endl;
        //TODO: Do not forget enable DMA and ither paramaters
        _pwm_regs->_rng1 = 32;
        std::this_thread::sleep_for(std::chrono::microseconds(100));

        ////std::cout << "C2 PWM STA  0x" << std::hex <<_pwm_regs->_sta << std::endl;
        _pwm_regs->_dmac = (RPI_PWM_DMAC_ENAB | RPI_PWM_DMAC_PANIC(7) | RPI_PWM_DMAC_DREQ(3));
        std::this_thread::sleep_for(std::chrono::microseconds(100));

        ////std::cout << "C3 PWM STA  0x" << std::hex <<_pwm_regs->_sta << std::endl;
        //Use FIFO and Serialiser mode for channel 1
        _pwm_regs->_ctl |= (PWM_CTRL_USEF1 | PWM_CTRL_MODE1);
        std::this_thread::sleep_for(std::chrono::microseconds(100));

        ////std::cout << "C4 PWM STA  0x" << std::hex <<_pwm_regs->_sta << std::endl;
        //Ebable channel 1
        _pwm_regs->_ctl |= PWM_CTRL_PWEN1;
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        ////std::cout << "C5 PWM STA  0x" << std::hex <<_pwm_regs->_sta << std::endl;
   }

    /*
    *
    */
    void _clear(){
        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__));

        if(!_is_init()){
            logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__) + " Not started");
            return;
        }

        //std::cout << " PwmCore _clear" << std::endl;

        _pwm_regs->_dmac = 0;   //Disable DMA

        //Stop PWM
        _pwm_regs->_ctl = 0;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        _pwm_regs->_ctl |= PWM_CTRL_CLRF1; //clear FIFO
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //clear error flags
        //std::cout << "E1 PWM STA  0x" << std::hex <<_pwm_regs->_sta << std::endl;
        _pwm_regs->_sta |= PWM_STA_BERR;
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        _pwm_regs->_sta |= (PWM_STA_RERR1 | PWM_STA_WERR1);
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //std::cout << " PwmCore _clear finished" << std::endl;
        //std::cout << "E3 PWM STA  0x" << std::hex <<_pwm_regs->_sta << std::endl;
    }

    const bool is_error() const {
        return ((_pwm_regs->_sta & PWM_STA_BERR) != 0);
    }

    void clear_berr(){
         _pwm_regs->_sta |= PWM_STA_BERR;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        //std::cout << "EE PWM STA  0x" << std::hex <<_pwm_regs->_sta << std::endl;
    }

    /*
    * List functions for changing control parameters
    */
    void _stop(){
            logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__));

            _clear();

            //Stop clock
            if( _is_clock() )
                _pwm_clock->_stop();

            logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__) + " Finished");
    }

    const bool _is_dma() const{
        return (_dma_ctrl != nullptr);
    }

    const bool _is_clock() const{
        return (_pwm_clock != nullptr);
    }

    volatile pwm_regs* _pwm_regs;          //PWM control & data registers
    pi_core::core_clock_pwm::PwmClock* _pwm_clock;  //PWM Clock object
    pi_core::core_dma::DmaControl* _dma_ctrl;       //DMA Control object
};


}//core_pwm
}//pi_core

#endif
