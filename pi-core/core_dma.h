/*
 * core_dma.h
 *
 * BCM2835 ARM Peripherals. DMA
 *
 *  Created on: March 12, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_CORE_DMA_H_
#define PI_CORE_DMA_H_

#include "core_common.h"
#include "smallthings.h"
#include "logger.h"

namespace pi_core {
namespace core_dma {

/*
* BCM2835 ARM peripherials (section 4.2.1)
*
*/
struct dma_ctrl_blk_t {
    uint32_t _ti;               //transfer information
    uint32_t _src_addr;         //source address
    uint32_t _dst_addr;         //destination address
    uint32_t _txfr_len;         //transfer length
    uint32_t _2d_mode_stride;   //2-D mode stride
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

/*
* DMA Control and Status register
*/
#define DMA_REG_CS_RESET    (1<<31)         //DMA Channel reset
#define DMA_REG_CS_ABORT    (1<<30)         //Abort DMA. Writing 1 will abort the current DMA CB
#define DMA_REG_CS_DISDEBUG (1<<29)         //Disable Debug
#define DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES (1<<28)
//bits 27:24 not used
#define DMA_REG_CS_PANIC_PRIORITY(val)  ((val & 0xf) << 20) //AXI panic priority level. Zero is lowest priority
#define DMA_REG_CS_PRIORITY(val)        ((val & 0xf) << 16) //AXI priority level. Zero is lovest priority
//bits 15:9 not used
#define DMA_REG_CS_ERROR    (1<<8)          //DMA Error, Read Only
//bit 7 not used
#define DMA_REG_CS_WAITING_FOR_OUTSTANDING_WRITES (1<<6) //Read Only
#define DMA_REG_CS_DREQ_STOPS_DMA   (1<<5)  //Indicates if the DMA is currently paused (1) due to DREQ been inactive, Read Only
#define DMA_REG_CS_PAUSED           (1<<4)  //Indicates if the DMA is carrently paused (1), Read Only
#define DMA_REG_CS_DREQ             (1<<3)  //Indicates the state of selected DREQ (Data Request) signal, Read Only
#define DMA_REG_CS_INT              (1<<2)  //Interrupt status
#define DMA_REG_CS_END              (1<<1)  //DMA Ennd Flag. Set when the transfer described by the current DMA CB is complete. Write 1 to clear.
#define DMA_REG_CS_ACTIVE           (1<<0)  //Activate the DMA

/*
* DMA Transfer information register
*/
#define DMA_REG_TI_NO_WIDE_BURSTS       (1<<26)               //Don't Do wide writes as a 2 beat burst (DK: Not clear for me)
#define DMA_REG_TI_WAITS(val)           ((val & 0xf)  << 21)  //Add wait cycles (number of dammy cycles burnt after each DMA read/write operation is completed)
#define DMA_REG_TI_PERMAP(val)          ((val & 0x1f) << 16)  //Peripheral mapping (see DREQ structure)
#define DMA_REG_TI_BURST_LENGTH(val)    ((val & 0xf)  << 12)  //Burst transfer length. Usually 0
#define DMA_REG_TI_SRC_IGNORE       (1<<11)                   //Ignore reads
#define DMA_REG_TI_SRC_DREQ         (1<<10)                   //Control Source Reads with DREQ (1 - The DREQ selected by PERMAP will gate the source read)
#define DMA_REQ_TI_SRC_WIDTH_128    (1<<9)                    //Source transfer width (1-128bit, 0-32bit)
#define DMA_REQ_TI_SRC_INC          (1<<8)                    //Source address increment (1-Yes, 0-No) Increment value is on depend of SRC_WIDTH
#define DMA_REG_TI_DEST_IGNORE      (1<<7)                    //Ignore reads
#define DMA_REG_TI_DEST_DREQ        (1<<6)                    //Control Destination Reads with DREQ (1 - The DREQ selected by PERMAP will gate the destination read)
#define DMA_REQ_TI_DEST_WIDTH_128   (1<<5)                    //Destination transfer width (1-128bit, 0-32bit)
#define DMA_REQ_TI_DEST_INC         (1<<4)                    //Destination address increment (1-Yes, 0-No) Increment value is on depend of DEST_WIDTH
#define DMA_REG_TI_WAIT_RESP        (1<<3)                    //Wait for a Write Respose (1-Wait, 0-Don't wait)
#define DMA_REG_TI_TDMODE_2D        (1<<1)                    //1 - 2D mode, 0 - linear mode
#define DMA_REG_TI_INTEN            (1<<0)                    //Interrupt Enable (1-Generate interrupt when transfer complited, 0-Do not generate Iterrupt)

/*
* DMA Debug register
*/
#define DMA_REG_DEBUG_READ_ERROR                (1<<2)
#define DMA_REG_DEBUG_FIFO_ERROR                (1<<1)
#define DMA_REG_DEBUG_READ_LAST_NOT_SET_ERROR   (1<<0)

/*
* DMA control block
*/
class DmaControlBlock {
public:
    DmaControlBlock(const DREQ dreq = DREQ::NO_required) : _dreq(dreq), _ti_flags(0) {
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        _ctrk_blk = static_cast<struct dma_ctrl_blk_t*>( malloc(sizeof(struct dma_ctrl_blk_t)) );
        if(_ctrk_blk == nullptr){
            logger::log(logger::LLOG::ERROR, "DmaCtrl", std::string(__func__) + " Could not allocate memory for DMA CB");
            return;
        }
        memset((void*)_ctrk_blk, 0x00, sizeof(struct dma_ctrl_blk_t));

        if( _dreq == DREQ::PWM){
            logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " Selected CI flags for PWM");
            set_ti_flags(ti_flags_pwm);
        }
        else if(_dreq == DREQ::NO_required){
            logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " Selected CI flags for no DREQ");
            set_ti_flags(_ti_flags_test);
        }
    }

    ~DmaControlBlock(){
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        if( _ctrk_blk){
            free((void*)_ctrk_blk);
        }
    }

    /*
    * Set default flags for TI register
    *
    * DK: Not clear why we use DMA_REG_TI_NO_WIDE_BURSTS here (TODO: check without)
    *
    * PWM flags: DMA_REG_TI_NO_WIDE_BURSTS | DMA_REG_TI_WAIT_RESP | DMA_REG_TI_DEST_DREQ | DMA_REG_TI_PERMAP(5) | DMA_REQ_TI_SRC_INC
    * Memory test flags: DMA_REG_TI_NO_WIDE_BURSTS | DMA_REG_TI_WAIT_RESP | DMA_REQ_TI_SRC_INC | DMA_REQ_TI_DEST_INC
    *
    */
    void set_ti_flags(const uint32_t ti_flags){
        _ti_flags = ti_flags;
    }

    const uint32_t ti_flags_pwm = DMA_REG_TI_NO_WIDE_BURSTS | DMA_REG_TI_WAIT_RESP |  DMA_REQ_TI_SRC_INC | DMA_REG_TI_DEST_DREQ | DMA_REG_TI_PERMAP(5);
    const uint32_t _ti_flags_test = DMA_REG_TI_NO_WIDE_BURSTS | DMA_REG_TI_WAIT_RESP | DMA_REQ_TI_SRC_INC | DMA_REG_TI_DEST_DREQ | DMA_REG_TI_PERMAP(0);

    /*
    * Initialize DMA control block
    *
    * txfr_len - length in bytes (!)
    */
    bool Initialize(const uintptr_t src_addr, const uintptr_t dest_addr, const uint16_t txfr_len){

        _ctrk_blk->_ti = _ti_flags;
        _ctrk_blk->_src_addr = src_addr;
        _ctrk_blk->_dst_addr = dest_addr;
        _ctrk_blk->_txfr_len = txfr_len;
        _ctrk_blk->_2d_mode_stride = 0;
        _ctrk_blk->_next_ctrl_blk = 0; //no next DMA CB
    }

private:
    volatile struct dma_ctrl_blk_t* _ctrk_blk;  //DMA control block
    DREQ _dreq;                              //Data request (DREQ)
    uint32_t _ti_flags;
};

/*
* DMA control class
*/
class DmaControl {
public:
    DmaControl(const unsigned short dma = 10) : _addr(0), _dma(dma), _started(false){
        assert(dma < 15);
        _addr = dma_address(dma);

        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " DMA channel: " + std::to_string(dma) + " addr: " + std::to_string(_addr));
    }

    virtual ~DmaControl(){
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " addr: " + std::to_string(_addr));
        if(_addr){
            _dma_regs = piutils::unmap_memory<struct dma_regs_t>(static_cast<struct dma_regs_t*>((void*)_dma_regs));
        }
    }

    static const uintptr_t dma_base = 0x00007000;
    static const uintptr_t dma_address(const unsigned short dma){
        assert(dma < 15);

        uintptr_t ph_address = CoreCommon::get_peripheral_address();
        if(dma == 15)
            return ph_address + 0x00e05000;

        return ph_address + dma_base + 0x100*dma;
    }

    /*
    * Initialize connection
    */
    bool Initialize() {
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        _dma_regs = piutils::map_memory<struct dma_regs_t>(_addr);
        if(_dma_regs == nullptr){
            logger::log(logger::LLOG::ERROR, "DmaCtrl", std::string(__func__) + " Fail to initialize DMA control");
            return false;
        }

        _dma_regs->_cs = 0;         //initialize DMA control and status
        _dma_regs->_txrf_len = 0;

        _dma_regs->_cs = DMA_REG_CS_RESET;  //Reset DMA
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //Clear Interrupt (INT) and DMA end (END) statuses
        _dma_regs->_cs = DMA_REG_CS_INT | DMA_REG_CS_END;
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        return true;
    }

    /*
    * Reset DMA
    */
    void reset(){
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        _dma_regs->_cs = DMA_REG_CS_RESET;  //Reset DMA
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //Clear Interrupt (INT) and DMA end (END) statuses
        _dma_regs->_cs = DMA_REG_CS_INT | DMA_REG_CS_END;
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        //clear error flags
        _dma_regs->_debug = DMA_REG_DEBUG_READ_ERROR | DMA_REG_DEBUG_FIFO_ERROR | DMA_REG_DEBUG_READ_LAST_NOT_SET_ERROR;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    /*
    * Start to process DMA Control block
    */
    bool process_control_block(const uintptr_t dma_control_block) {
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        //Check did we finis process the current DMA CB
        if( !is_finished() ){
            logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " DMA is still busy");
            return false;
        }

        reset();

        _started = true;

        //set DMA Control Block
        _dma_regs->_conblk_ad = dma_control_block;
        _dma_regs->_cs = _cs_flags | DMA_REG_CS_ACTIVE;

        return true;
   }

   const bool is_error() const {
     return ((_dma_regs->_cs & DMA_REG_CS_ERROR) != 0);
   }

   const bool is_paused() const {
     return ((_dma_regs->_cs & DMA_REG_CS_PAUSED) != 0);
   }

   const bool is_waiting_for_outstanding_writes() const {
     return ((_dma_regs->_cs & DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES) != 0);
   }

    /*
    * Check if we finished to process current DMA CB
    */
    bool is_finished() {
        bool result = true;
        if(_started){
            result = ((_dma_regs->_cs & DMA_REG_CS_END) != 0);
            if(result){
                 _started = false;
                 logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " Clear started flag");
            }
        }

        return result;
    }

    /*
    * Set default flags for CS register
    *
    * flags PWM: DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES | DMA_REG_CS_PANIC_PRIORITY(15) | DMA_REG_CS_PRIORITY(15)
    * flag memory test: DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES
    *
    */
    void set_cs_flags(const uint32_t cs_flags){
        _cs_flags = cs_flags;
    }

    static const uint32_t cs_flags_pwm = DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES | DMA_REG_CS_PANIC_PRIORITY(15) | DMA_REG_CS_PRIORITY(15);
    static const uint32_t cs_flags_test = DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES;

private:
    uintptr_t _addr; //DMA register address
    uint16_t _dma;  //DMA number
    uint32_t _cs_flags; //default flags for CS register

    bool _started;  //Flag do we have active Control Block on processing
    volatile struct dma_regs_t* _dma_regs;
};


}
}

#endif
