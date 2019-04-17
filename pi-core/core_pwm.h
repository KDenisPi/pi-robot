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
#include "core_dma.h"
#include "core_clock_pwm.h"
#include "logger.h"

namespace pi_core {
namespace core_pwm {

#define DEV_MEM     "/dev/mem"

#define PWM_OFFSET                               (0x0020c000)   //use peripheral address as base for this address
#define PWM_PERIPH_PHYS                          (0x7e20c000)   //use this addrewss directly

struct pwm_regs_t {
    uint32_t _ctl;  //PWM Control
    uint32_t _sta;  //PWM Status
    uint32_t _dmac; //PWM DMA configuration
    uint32_t _rng1; //PWM channel 1 range
    uint32_t _dat1; //PWM channel 1 data
    uint32_t _fif1; //PWM FIFO input. This register is the FIFO input for the all channels.
    uint32_t _rng2; //PWM channel 2 range
    uint32_t _dat2; //PWM channel 2 data
} __attribute__((packed, aligned(4))) pwm_regs;


/*
* PWM Control register (CTL)
*/
//bits 31-16 not used, write 0
#define PWM_CTRL_MSEN2 0x00008000    // RW bit 15; Channel 1 M/S Enable; (0-PWM algorithm used; 1-M/S transmission used)
//bit 14-not used, write 0
#define PWM_CTRL_USEF2  0x00002000   // RW bit 13; Channel 2 use FIFO (0-Data register is transmitted; 1-FIFO used for transmission)
#define PWM_CTRL_POLA2  0x00001000   // RW bit 12; Channel 2 polarity; (0-[0:low; 1-high], 1-[1-low; 0-high])
#define PWM_CTRL_SBIT2  0x00000800   // RW bit 11; Channel 2 silence bit; Defines the state of the output when no transmission takes place
#define PWM_CTRL_RPTL2  0x00000400   // RW bit 10; Channel 2 repeat last data; (0-Transmission interrupts when FIFO is empty,
                                     //                                        1-Last data in FIFO is transmitted repetedly until FIFO is not empty)
#define PWM_CTRL_MODE2  0x00000200   // RW bit 9; Channel 2 mode; (0-PWM mode, 1-Serialiser mode)
#define PWM_CTRL_PWEN2  0x00000100   // RW bit 8; Channel 2 enable (0-disable, 1-enable)

#define PWM_CTRL_MSEN1  0x00000080   // RW bit 7; Channel 1 M/S Enable; (0-PWM algorithm used; 1-M/S transmission used)
#define PWM_CTRL_CLRF1  0x00000040   // RO bit 6; (1-Clears FIFO, 0-Has no effect; This is single shot operation)
                                     // This bit always reads 0
#define PWM_CTRL_USEF1  0x00000020   // RW bit 5; Channel 1 use FIFO (0-Data register is transmitted; 1-FIFO used for transmission)
#define PWM_CTRL_POLA1  0x00000010   // RW bit 4; Channel 1 polarity; (0-[0:low; 1-high], 1-[1-low; 0-high])
#define PWM_CTRL_SBIT1  0x00000008   // RW bit 3; Channel 1 silence bit; Defines the state of the output when no transmission takes place
#define PWM_CTRL_RPTL1  0x00000004   // RW bit 2; Channel 1 repeat last data; (0-Transmission interrupts when FIFO is empty,
                                     //                                        1-Last data in FIFO is transmitted repetedly until FIFO is not empty)
#define PWM_CTRL_MODE1  0x00000002   // RW bit 1; Channel 1 mode; (0-PWM mode, 1-Serialiser mode)
#define PWM_CTRL_PWEN1  0x00000001   // RW bit 0; Channel 1 enable (0-disable, 1-enable)

/*
* PWM Status register (STA)
*/
#define PWM_STA_STA4    0x00001000   // RW bit 12; Channel 4 State
#define PWM_STA_STA3    0x00000800   // RW bit 11; Channel 3 State
#define PWM_STA_STA2    0x00000400   // RW bit 10; Channel 2 State
#define PWM_STA_STA1    0x00000200   // RW bit  9; Channel 1 State
#define PWM_STA_BERR    0x00000100   // RW bit 8; Bus error flag
#define PWM_STA_GAPO4   0x00000080   // RW bit 7; Channel 4 Gap occurred flag
#define PWM_STA_GAPO3   0x00000040   // RW bit 6; Channel 3 Gap occurred flag
#define PWM_STA_GAPO2   0x00000020   // RW bit 5; Channel 2 Gap occurred flag
#define PWM_STA_GAPO1   0x00000010   // RW bit 4; Channel 1 Gap occurred flag
#define PWM_STA_RERR1   0x00000008   // RW bit 3; FIFO Read error flag
#define PWM_STA_WERR1   0x00000004   // RW bit 2; FIFO Write error flag
#define PWM_STA_EMPT1   0x00000002   // RW bit 1; FIFO Empty flag
#define PWM_STA_FULL1   0x00000001   // RW bit 0; FIFO full flag

/*
* PWM DMA Configuration register (DMAC)
*/
#define RPI_PWM_DMAC_ENAB        0x80000000          //RW bit  31; DMA Enable; (0-DMA disabled; 1-DMA enabled)
#define RPI_PWM_DMAC_PANIC_RESET 0x00000700          //RW bits 7-0; DMA Treshold for PANIC signal (reset value 0x7)
#define RPI_PWM_DMAC_PANIC(val)  ((val & 0xff) << 8) //RW bits 15-8; DMA Treshold for PANIC signal (reset value 0x7)
#define RPI_PWM_DMAC_DREQ_RESET  0x07                //RW bits 7-0; DMA Treshold for DREQ signal (reset value 0x7)
#define RPI_PWM_DMAC_DREQ(val)   (val & 0xff)        //RW bits 7-0; DMA Treshold for DREQ signal (reset value 0x7)

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




class PwmCore
{
public:
    PwmCore() : _pwm_regs(nullptr), _pwm_clock(nullptr), _dma_ctrl(nullptr)
    {
        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__));
    /*
        _pwm_clock = new pi_core::core_clock_pwm::PwmClock();
        _dma_ctrl = new pi_core::core_dma::DmaControl(10); //TODO DMA channel
    */
        uint32_t ph_address = pi_core::CoreCommon::get_peripheral_address();
        std::cout << " (ph_address + pwmctl_addr): " <<  std::hex << (ph_address + pwmctl_addr) << std::endl;

        _pwm_regs = piutils::map_memory<struct pwm_regs_t>(ph_address + pwmctl_addr);
        if( _pwm_regs == nullptr){
            logger::log(logger::LLOG::ERROR, "PwmCore", std::string(__func__) + " Fail to initialize PWM control");
        }

        std::cout << " _pwm_regs: " <<  std::hex << (void*)_pwm_regs << std::endl;
    }

    virtual ~PwmCore(){
        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__));

        if(_pwm_regs)
            piutils::unmap_memory<struct pwm_regs_t>((struct pwm_regs_t*)_pwm_regs);

        if(_dma_ctrl){
            delete _dma_ctrl;
        }

        if(_pwm_clock){
            delete _pwm_clock;
        }
    }

    virtual bool write_data(unsigned char* data, int len){
        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__) + " Write to PWM: " +  std::to_string(len));
        return true;
    }

    const uint32_t pwmctl_addr = PWM_OFFSET;
    const uint32_t pwmctl_addr_phys = PWM_PERIPH_PHYS;

    const uint32_t pwmctl_frequency = 19200000;

    /*
    * Initialize connection
    */
   bool Initialize() {

       _stop();

/*
        if( !_pwm_clock->Initialize()){
            logger::log(logger::LLOG::ERROR, "PwmCore", std::string(__func__) + " Fail to initialize PWM Clock");
            return false;
        }

        if( !_dma_ctrl->Initialize()){
            logger::log(logger::LLOG::ERROR, "PwmCore", std::string(__func__) + " Fail to initialize DMA Control");
            return false;
        }
*/
        return true;
    }

    /*
    * Get physical address of FIFO register
    */
   const uintptr_t get_fifo_address() const {
        return (uintptr_t)&((struct pwm_regs_t*)PWM_PERIPH_PHYS)->_fif1;
   }

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

   }

    /*
    * List functions for changing control parameters
    */
   void _stop(){
        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__));

        if(!_is_init())
            return;

        std::cout << " PwmCore 0 CTL: " <<  std::hex << _pwm_regs->_ctl << std::endl;

        //Stop PWM
        _pwm_regs->_ctl = 0;
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        std::cout << " PwmCore 1 CTL: " <<  std::hex << _pwm_regs->_ctl << std::endl;

        //Stop clock
        //_pwm_clock->_stop();

        logger::log(logger::LLOG::DEBUG, "PwmCore", std::string(__func__) + " Finished");
   }

    /*
    * Get string with CS register status
    */
    const std::string cs_register_status() {
        return _dma_ctrl->cs_register_status();
    }

    /*
    * Get string with CS register status
    */
    const std::string ti_register_status() {
        return _dma_ctrl->ti_register_status();
    }

private:
    volatile struct pwm_regs_t* _pwm_regs; //control & data registers
    pi_core::core_clock_pwm::PwmClock* _pwm_clock;
    pi_core::core_dma::DmaControl* _dma_ctrl;
};


}//core_pwm
}//pi_core

#endif
