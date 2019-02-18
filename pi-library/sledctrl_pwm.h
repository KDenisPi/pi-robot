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
#define CLOCK_PWMCTL_OFFSET      0x001010a0      //Clock Manager Audio Clock Control. Registers

#define CLOCK_PWMDIV_PHYS        0x7e1010a4      //Clock Manager Audio Clock Divisors
#define CLOCK_PWMDIV_OFFSET      0x001010a4      //Clock Manager Audio Clock Divisors. Registers

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
* BCM2835 ARM peripherials (section 4.2.1)
*
*/
struct dma_ctrl_blk_t {
    uint32_t _ti;       //transfer information
    uint32_t _src_addr; //source address
    uint32_t _dst_addr; //destination address
    uint32_t _trans_len;//transfer length
    uint32_t _2d_mode_stride; //2-D mode stride
    uint32_t _next_ctrl_blk;    //Next control block
    uint32_t _reserved1;
    uint32_t _reserved2;
}  __attribute__((packed, aligned(4))) dma_ctrl_blk;

struct dma_regs_t{
    uint32_t _cs;           //DMA control and status
    uint32_t _conblk_ad;    //Control block address
    uint32_t _ti;           //Transfer information (Word 0)
    uint32_t _source_ad;    //Source address (Word 1)
    uint32_t _dest_ad;      //Destination address (Word 2)
    uint32_t _txrf_len;     //Transfer length (Word 3)
    uint32_t _stride;       //2D Stride (Word 4)
    uint32_t _nextconbk;    //Next DMA control block
    uint32_t _debug;        //Debug
}  __attribute__((packed, aligned(4))) dma_regs;

#define DMA_REG_CS_RESET    (1<<31)         //DMA Channel reset
#define DMA_REG_CS_ABORT    (1<<30)         //Abort DMA. Writing 1 will abort the current DMA CB
#define DMA_REG_CS_DISDEBUG (1<<29)         //Disable Debug
#define DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES (1<<28)
//bits 27:24 not used
#define DMA_REG_CS_PANIC_PRIORITY(val)  ((val & 0xf) << 20)
#define DMA_REG_CS_PRIORITY(val)  ((val & 0xf) << 16)
//bits 15:9 not used
#define DMA_REG_CS_ERROR    (1<<8)          //DMA Error, Read Only
//bit 7 not used
#define DMA_REG_CS_WAITING_FOR_OUTSTANDING_WRITES (1<<6) //Read Only
#define DMA_REG_CS_DREQ_STOPS_DMA   (1<<5)  //Indicates if the DMA is currently paused (1) due to DREQ been inactive, Read Only
#define DMA_REG_CS_PAUSED           (1<<4)  //Indicates if the DMA is carrently paused (1), Read Only
#define DMA_REG_CS_DREQ             (1<<3)  //Indicates the state of selected DREQ (Data Request) signal, Read Only
#define DMA_REG_CS_INT              (1<<2)  //Interrupt status
#define DMA_REG_CS_END              (1<<1)  //DMA Eend Flag
#define DMA_REG_CS_ACTIVE           (1<<0)  //Activate the DMA


class SLedCtrlPwm;

/*
* DMA control class
*/
class DmaControl {
public:
    DmaControl(const unsigned short dma) : _addr(0), _dma(dma){
        assert( dma < 15);

        _addr = dma_address(dma);
    }

    virtual ~DmaControl(){
        if(_addr){
            _dma_regs = piutils::unmap_memory<struct dma_regs_t>(static_cast<struct dma_regs_t*>((void*)_dma_regs));
        }
    }

    friend SLedCtrlPwm;

    static const uint32_t dma_base = 0x00007000;
    static const uint32_t dma_address(const unsigned short dma){
        uint32_t ph_address = Item::get_peripheral_address();
        if(dma == 15)
            return ph_address + 0x00e05000;

        return ph_address + dma_base + 0x100*dma;
    }

    /*
    * Initialize connection
    */
   bool Initialize() {
        _dma_regs = piutils::map_memory<struct dma_regs_t>(_addr);
        if(_dma_regs == nullptr){
            logger::log(logger::LLOG::ERROR, "DmaCtrl", std::string(__func__) + " Fail to initialize DMA control");
            return false;
        }
       return true;
   }

private:
    uint32_t _addr; //DMA register address
    uint16_t _dma;  //DMA number
    volatile struct dma_regs_t* _dma_regs;
};

/*
* PWM Clock control class
*/
class PwmClock{
public:
    PwmClock() : _pwm_clock(nullptr) {
    }

    virtual ~PwmClock() {
        if(_pwm_clock)
            _pwm_clock = piutils::unmap_memory<struct pwm_clock_t>(static_cast<struct pwm_clock_t*>((void*)_pwm_clock));
    }

    const uint32_t clock_pwmctl = CLOCK_PWMCTL_OFFSET;
    const uint32_t clock_pwmctl_phys = CLOCK_PWMCTL_PHYS;

    friend SLedCtrlPwm;

    /*
    * Initialize connection
    */
   bool Initialize() {
        uint32_t ph_address = Item::get_peripheral_address();
        _pwm_clock = piutils::map_memory<struct pwm_clock_t>(ph_address + clock_pwmctl);
        if(_pwm_clock == nullptr){
            logger::log(logger::LLOG::ERROR, "PwmClock", std::string(__func__) + " Fail to initialize PWM Clock ");
            return false;
        }
        return true;
    }

    const bool _is_init() const{
        return (_pwm_clock != nullptr);
    }

protected:

    /*
    * List functions for changing control parameters
    */
    void _stop() {
        if(!_is_init()) return;

        _pwm_clock->_pwmctl = (CM_CLK_CTL_KILL | CM_CLK_CTL_PASSWD);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        //TODO: delay too?
        while(_is_busy());
    }

    const bool _is_busy() const {
        return ((_pwm_clock->_pwmctl & CM_CLK_CTL_BUSY) != 0);
    }

    /*
    * Set initial values for PWM Clock
    */
   void _configure(const uint32_t base_freq, const uint32_t freq) {
        _pwm_clock->_pwmdiv = CM_CLK_DIV_PASSWD | CM_CLK_DIV_DIVI(base_freq / (3 * freq));
        _pwm_clock->_pwmctl = CM_CLK_CTL_PASSWD | CM_CLK_CTL_SRC_OSC;
        _pwm_clock->_pwmctl = CM_CLK_CTL_PASSWD | CM_CLK_CTL_SRC_OSC | CM_CLK_CTL_ENAB;

        std::this_thread::sleep_for(std::chrono::microseconds(10));
        while(_is_busy());
   }


private:
    volatile struct pwm_clock_t* _pwm_clock;
};

class SLedCtrlPwm : public pirobot::item::sledctrl::SLedCtrl
{
public:
    SLedCtrlPwm(const int sleds,
        const std::string& name,
        const std::string& comment="") :
            SLedCtrl(item::ItemTypes::SLEDCRTLPWM, sleds, name, comment), _pwm_regs(nullptr)
    {
        logger::log(logger::LLOG::DEBUG, "SLedCtrlPwm", std::string(__func__) + " name " + name);
        _pwm_clock = new PwmClock();
        _dma_ctrl = new DmaControl(5); //TODO DMA channel
    }

    virtual ~SLedCtrlPwm(){
        logger::log(logger::LLOG::DEBUG, "SLedCtrlPwm", std::string(__func__));

        delete _dma_ctrl;
        delete _pwm_clock;
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

    const uint32_t pwmctl_addr = PWM_OFFSET;
    const uint32_t pwmctl_addr_phys = PWM_PERIPH_PHYS;

    const uint32_t pwmctl_frequency = 19200000;

    /*
    * Initialize connection
    */
   bool Initialize() {

        if( !_pwm_clock->Initialize()){
            logger::log(logger::LLOG::ERROR, "SLedCtrlPwm", std::string(__func__) + " Fail to initialize PWM Clock");
            return false;
        }

        if( !_dma_ctrl->Initialize()){
            logger::log(logger::LLOG::ERROR, "SLedCtrlPwm", std::string(__func__) + " Fail to initialize DMA Control");
            return false;
        }

        uint32_t ph_address = Item::get_peripheral_address();
        _pwm_regs = piutils::map_memory<struct pwm_regs_t>(ph_address + pwmctl_addr);
        if( _pwm_regs == nullptr){
            logger::log(logger::LLOG::ERROR, "SLedCtrlPwm", std::string(__func__) + " Fail to initialize PWM control");
            return false;
        }

        return true;
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

protected:

    /*
    * List functions for changing control parameters
    */
   void _stop(){
        if(!_is_init()) return;

        //Stop PWM
        _pwm_regs->_ctl = 0;
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //Stop clock
        _pwm_clock->_stop();
   }

private:
    volatile struct pwm_regs_t* _pwm_regs; //control & data registers
    PwmClock* _pwm_clock;
    DmaControl* _dma_ctrl;
};


}
}
}

#endif