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

    static const uint32_t dma_base = 0x00007000;
    static const uint32_t dma_address(const unsigned short dma){
        uint32_t ph_address = CoreCommon::get_peripheral_address();
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


}
}

#endif
