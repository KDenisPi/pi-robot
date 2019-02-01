/*
 * sledctrl_pwm.h
 *
 * PWM based LED Stripe Controller
 *
 *  Created on: Jan 15, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SLED_CTRL_PWM_H_
#define PI_LIBRARY_SLED_CTRL_PWM_H_

#include "sledctrl.h"
#include "smallthings.h"
#include "logger.h"

namespace pirobot {
namespace item {
namespace sledctrl {

#define DEV_MEM     "/dev/mem"

#define CLOCK_PWMCTL_PHYS        0x7e1010a0      //Clock Manager Audio Clock Control
#define CLOCK_PWMCTL             0x001010a0      //Clock Manager Audio Clock Control. Registers

#define CLOCK_PWMDIV_PHYS        0x7e1010a4      //Clock Manager Audio Clock Divisors
#define CLOCK_PWMDIV             0x001010a4      //Clock Manager Audio Clock Divisors. Registers

struct pwm_clock_t {
    uint32_t _pwmctl;       //Clock Manager Audio Clock Control register
    uint32_t _pwmdiv;       //Clock Manager Audio Clock Divisors register
} __attribute__((packed, aligned(4))) pwm_clock;

#define CM_CLK_CTL_SRC_GND                          0x00000000  //RW bit 0-3; Clock source; GND (reset 0)
#define CM_CLK_CTL_SRC_OSC                          0x00000001  //RW bit 0-3; Clock source; Oscillator
#define CM_CLK_CTL_SRC_TSTDBG0                      0x00000002  //RW bit 0-3; Clock source; testdebug0
#define CM_CLK_CTL_SRC_TSTDBG1                      0x00000003  //RW bit 0-3; Clock source; testdebug1
#define CM_CLK_CTL_SRC_PLLA                         0x00000004  //RW bit 0-3; Clock source; PLLA per
#define CM_CLK_CTL_SRC_PLLC                         0x00000005  //RW bit 0-3; Clock source; PLLC per
#define CM_CLK_CTL_SRC_PLLD                         0x00000006  //RW bit 0-3; Clock source; PLLD per
#define CM_CLK_CTL_SRC_HDMIAUX                      0x00000007  //RW bit 0-3; Clock source; HDMI auxiliary
#define CM_CLK_CTL_SRC_GNDS                         0x00000008  //RW bit 0-3; Clock source; 8-15 GND
#define CM_CLK_CTL_ENAB                             0x00000010  //RW bit 4; Enable clock generator (1-enable;0-reset)
#define CM_CLK_CTL_KILL                             0x00000020  //RW bit 5; Kill clock generator (1-stop and reset the clock generator;0-no action)
//-- bit 6 unused
#define CM_CLK_CTL_BUSY                             0x00000080  //RO bit 7; Clock generator is running (1-running;0-reset)
#define CM_CLK_CTL_FLIP                             0x00000100  //RW bit 8; Invert the clock generator output (1-invert;0-reset)
#define CM_CLK_CTL_MASH_IDIV                        0x00000000  //RW bit 10-9; Integer division 0
#define CM_CLK_CTL_MASH_1STAGE                      0x00000200  //RW bit 10-9; Integer division 0
#define CM_CLK_CTL_MASH_2STAGE                      0x00000400  //RW bit 10-9; Integer division 0
#define CM_CLK_CTL_MASH_3STAGE                      0x00000600  //RW bit 10-9; Integer division 0

#define CM_CLK_CTL_PASSWD                           0x5a000600  //W bit 31-24; Clock manager password '5a'

#define CM_CLK_DIV_PASSWD                           0x5a000600  //W bit 31-24; Clock manager password '5a'
#define CM_CLK_DIV_DIVI(val)   ((val & 0x0FFF) << 12)           //RW bir 23-12; Integer part of divisor
#define CM_CLK_DIV_DIVF(val)   (val & 0x0FFF)                   //RW bir 11-0; Fractional part of divisor

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

/*
* PWM Clock control class
*/
class PwmClock{
public:
    PwmClock() {
        Initialize();
    }

    virtual ~PwmClock() {
        _pwm_clock = piutils::unmap_memory<struct pwm_clock_t>(static_cast<struct pwm_clock_t*>((void*)_pwm_clock));
    }

    const uint32_t clock_pwmctl = 0x001010a0;
    const uint32_t clock_pwmctl_phys = CLOCK_PWMCTL_PHYS;

    /*
    * Initialize connection
    */
   bool Initialize() {
        uint32_t ph_address = Item::get_peripheral_address();
        _pwm_clock = piutils::map_memory<struct pwm_clock_t>( ph_address + clock_pwmctl);
   }

    /*
    * List functions for changing contril parameters
    */


private:
    volatile struct pwm_clock_t* _pwm_clock;
};

class SLedCtrlPwm : public pirobot::item::sledctrl::SLedCtrl
{
public:
    SLedCtrlPwm(const int sleds,
        const std::string& name,
        const std::string& comment="") :
            SLedCtrl(item::ItemTypes::SLEDCRTLPWM, sleds, name, comment)
    {
        logger::log(logger::LLOG::DEBUG, "SLedCtrlPwm", std::string(__func__) + " name " + name);
    }

    virtual bool write_data(unsigned char* data, int len) override {
        logger::log(logger::LLOG::DEBUG, "SLedCtrlPwm", std::string(__func__) + " Write to PWM: " +  std::to_string(len));
        return true;
    }

    /*
    * Print device configuration
    */
    virtual const std::string printConfig() override {
        std::string result =  name() + "\n";
        result += SLedCtrl::printConfig();

        return result;
    }

    /*
    * ON for PWM based device
    */
    virtual void On() override{

    }

    /*
    * OFF for PWM based device
    */
    virtual void Off() override{

    }

private:

private:
    struct pwm_regs_t _regs; //control & data registers

    PwmClock _pwm_clock;
};


}
}
}

#endif