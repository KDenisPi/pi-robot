/*
 * core_dma.h
 *
 * BCM2835 ARM Peripherals. DMA
 *
 *  Created on: March 12, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_CORE_DIRECT_MEMORY_ACCESS_H_
#define PI_CORE_DIRECT_MEMORY_ACCESS_H_

#include "core_memory.h"
#include "smallthings.h"
#include "logger.h"

namespace pi_core {

namespace core_pwm {
    class PwmCore;
}

namespace core_dma {

class DmaControl;
/*
* BCM2835 ARM peripherials (section 4.2.1)
*
*/
using dma_ctrl_blk = struct __attribute__((packed, aligned(4))) dma_ctrl_blk_t {
    uint32_t _ti;               //transfer information
    uint32_t _src_addr;         //source address
    uint32_t _dst_addr;         //destination address
    uint32_t _txfr_len;         //transfer length
    uint32_t _2d_mode_stride;   //2-D mode stride
    uint32_t _next_ctrl_blk;    //Next control block
    uint32_t _reserved1;
    uint32_t _reserved2;
};

using dma_regs = struct __attribute__((packed, aligned(4))) dma_regs_t {
    uint32_t _cs;           //DMA control and status
    uint32_t _conblk_ad;    //Control block address
    uint32_t _ti;           //Transfer information (Word 0)
    uint32_t _source_ad;    //Source address (Word 1)
    uint32_t _dest_ad;      //Destination address (Word 2)
    uint32_t _txrf_len;     //Transfer length (Word 3)
    uint32_t _stride;       //2D Stride (Word 4)
    uint32_t _nextconbk;    //Next DMA control block
    uint32_t _debug;        //Debug
};

using dma_ctrl_regs = struct __attribute__((packed, aligned(4))) dma_regs_ctrlt {
    uint32_t _int_status;  //Internal status of each DMA channel
    uint32_t _reserved[3];
    uint32_t _enable;      //Global enable bits for each DMA channel
};


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
    DmaControlBlock(pi_core::core_mem::PhysMemory* mem_alloc, DREQ dreq = DREQ::PWM) :
        _mem_alloc(mem_alloc), _dreq(dreq), _ti_flags(0)
    {
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        std::cout << "**** DMA Control Block Condtructor " << std::endl;

        _minfo = _mem_alloc->get_memory(sizeof(dma_ctrl_blk));
        if(_minfo->is_empty()){
            logger::log(logger::LLOG::ERROR, "DmaCtrl", std::string(__func__) + " Could not allocate memory for DMA CB");
            throw std::runtime_error(std::string("Could not allocate memory for DMA CB"));
        }

        _ctrk_blk = static_cast<volatile dma_ctrl_blk*>(_minfo->get_vaddr());

        //initialize memory
        memset((void*)_ctrk_blk, 0x00, sizeof(dma_ctrl_blk));

        if( _dreq == DREQ::PWM){
            logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " Selected CI flags for PWM");
            set_ti_flags(_ti_flags_pwm);
        }
        else if(_dreq == DREQ::NO_required){ //TODO: Is there any way to use it?
            logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " Selected CI flags for no DREQ");
            set_ti_flags(_ti_flags_test);
        }
    }

    ~DmaControlBlock(){
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        std::cout << "**** DMA Control Block Destructor " << std::endl;
        if( _mem_alloc){
            _mem_alloc->free_memory(_minfo);
        }
    }

    friend class pi_core::core_pwm::PwmCore;
    friend class DmaControl;

public:
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

    const uint32_t _ti_flags_pwm = DMA_REG_TI_NO_WIDE_BURSTS | DMA_REG_TI_WAIT_RESP |  DMA_REQ_TI_SRC_INC | DMA_REG_TI_DEST_DREQ | DMA_REG_TI_PERMAP(5);
    const uint32_t _ti_flags_test = DMA_REQ_TI_SRC_INC | DMA_REQ_TI_DEST_INC | DMA_REG_TI_NO_WIDE_BURSTS;

    /*
    * Initialize DMA control block
    *
    * txfr_len - length in bytes (!)
    */
    bool prepare(const uintptr_t src_addr, const uintptr_t dest_addr, const uint16_t txfr_len){

        std::cout << "**** DMA prepare Src: 0x" << std::hex << src_addr << " Dst: 0x" << std::hex << dest_addr << " Len: " << std::dec << txfr_len << std::endl;

        _ctrk_blk->_ti = _ti_flags;
        _ctrk_blk->_src_addr = src_addr;
        _ctrk_blk->_dst_addr = dest_addr;
        _ctrk_blk->_txfr_len = txfr_len;
        _ctrk_blk->_2d_mode_stride = 0;
        _ctrk_blk->_next_ctrl_blk = 0; //no next DMA CB

        print();

       return true;
    }

protected:
    void print() const {
        std::cout << "DMA Addr: " << std::hex << (void*)_ctrk_blk << std::endl <<  "DMA PAddr: 0x" << std::hex << get_phys_ctrl_blk() << std::endl <<
            " TI: 0x" << _ctrk_blk->_ti << " SRC: 0x" << _ctrk_blk->_src_addr << " DST: 0x" << _ctrk_blk->_dst_addr <<
            " txfr_len: 0x" << _ctrk_blk->_txfr_len << " 2D Stride: 0x" << _ctrk_blk->_2d_mode_stride << " Next: 0x " << _ctrk_blk->_next_ctrl_blk << std::endl;
    }

    const volatile dma_ctrl_blk* get_ctrl_blk() const {
        return _ctrk_blk;
    }

    const uintptr_t get_phys_ctrl_blk() const {
        return _minfo->get_paddr();
    }

private:
    volatile dma_ctrl_blk* _ctrk_blk;  //DMA control block
    DREQ _dreq;               //Data request (DREQ)
    uint32_t _ti_flags;

    pi_core::core_mem::PhysMemory* _mem_alloc;
    std::shared_ptr<pi_core::core_mem::MemInfo> _minfo;
};

/*
* DMA info class
*
*/
class DmaInfo {
public:
    /*
     */
    DmaInfo() {
        _addr_ctrl = dma_int_status_reg();
        std::cout <<  "DmaCtrl DMA Control: 0x" << std::hex << _addr_ctrl << std::endl;

        _dma_ctrl_regs = piutils::map_memory<dma_ctrl_regs>(_addr_ctrl);
        if(_dma_ctrl_regs == nullptr){
            logger::log(logger::LLOG::ERROR, "DmaCtrl", std::string(__func__) + " Fail to initialize DMA control");
            throw std::runtime_error(std::string("Fail to initialize DMA control register"));
        }

        std::cout <<  "DmaCtrl DMA Control: INT_STATUS 0x" << std::hex << _dma_ctrl_regs->_int_status << " Enable: " <<  _dma_ctrl_regs->_enable  << std::endl;
    }

    /*
     */
    virtual ~DmaInfo() {
        _dma_ctrl_regs = piutils::unmap_memory<dma_ctrl_regs>(static_cast<dma_ctrl_regs*>((void*)_dma_ctrl_regs));

    }

    //
    static const uintptr_t dma_base = 0x00007000;

    /*
    *
    */
    static const uintptr_t dma_address(const uint16_t dma){
        assert(dma < 16);

        uintptr_t ph_address = CoreCommon::get_peripheral_address();
        if(dma == 15)
            return ph_address + 0xe05000;

        return ph_address + dma_base + (0x100 * dma);
    }

    /*
    *
    */
    static const uintptr_t dma_enable_reg(){
        uintptr_t ph_address = CoreCommon::get_peripheral_address();
        return ph_address + (dma_base | 0xFF0);
    }


    /*
    *
    */
    static const uintptr_t dma_int_status_reg(){
        uintptr_t ph_address = CoreCommon::get_peripheral_address();
        return ph_address + (dma_base | 0xFE0);
    }

    /*
    * Check DMA channel status
    */
   const bool dma_channel_check(const uint16_t dma_channel){

        volatile dma_regs* dma_reg;
        uintptr_t dma_addr = dma_address(dma_channel);

        std::cout << std::endl <<  "-----> DMA channel: " << std::dec << dma_channel <<  " addr: 0x" << std::hex << dma_addr << " Enabled: " << is_enabled(dma_channel) << std::endl;

        dma_reg = piutils::map_memory<dma_regs>(dma_addr);
        if(dma_reg == nullptr){
            //logger::log(logger::LLOG::ERROR, "DmaCtrl", std::string(__func__) + " Fail to initialize DMA control");
            throw std::runtime_error(std::string("Fail to initialize DMA control"));
        }

        std::cout <<  cs_register_status(dma_reg->_cs) << std::endl << std::endl;
        std::cout <<  ti_register_status(dma_reg->_ti) << std::endl << std::endl;


        dma_reg = piutils::unmap_memory<dma_regs>(static_cast<dma_regs*>((void*)dma_reg));
    }

    /*
    *
    */
    const bool is_enabled(const uint16_t dma_channel) const{
        return ( (_dma_ctrl_regs->_enable & (1 << dma_channel)) != 0 );
    }

    /*
    *
    */
    void dma_channel_enable(const uint16_t dma_channel){
      uint32_t dma_bit = (1 << dma_channel);
      uint32_t dma_enb = _dma_ctrl_regs->_enable;

      std::cout <<  "dma_channel_enable: INT_STATUS 0x" << std::hex << _dma_ctrl_regs->_int_status << " Enable: " <<  _dma_ctrl_regs->_enable  << "  DMA:" << dma_channel << std::endl;

      if( (dma_enb&dma_bit) == 0 )
        _dma_ctrl_regs->_enable |= dma_bit;

      usleep(100);
    }

    /*
    * Get string with CS register status
    */
    static const std::string cs_register_status(const uint32_t cs_state) {
        char buff[64];
        std::string res = "CS register: ";
        std::sprintf(buff, "0x%X\n", cs_state);
        res += buff;
        res += CoreCommon::bit_test(buff, cs_state, 0, "ACTIVE");
        res += CoreCommon::bit_test(buff, cs_state, 1, "END");
        res += CoreCommon::bit_test(buff, cs_state, 2, "INT");
        res += CoreCommon::bit_test(buff, cs_state, 3, "DREQ");
        res += CoreCommon::bit_test(buff, cs_state, 4, "PAUSED");
        res += CoreCommon::bit_test(buff, cs_state, 5, "DREQ_STOPS_DMA");
        res += CoreCommon::bit_test(buff, cs_state, 6, "WAITING_FOR_OUTSTANDING_WRITES");
        res += CoreCommon::bit_test(buff, cs_state, 8, "ERROR");
        res += CoreCommon::bits_test(buff,  ((cs_state>>16)&0xF), "PRIORITY");
        res += CoreCommon::bits_test(buff,  ((cs_state>>20)&0xF), "PANIC PRIORITY");
        res += CoreCommon::bit_test(buff, cs_state, 28, "WAIT_FOR_OUTSTANDING_WRITES");
        res += CoreCommon::bit_test(buff, cs_state, 29, "DISDEBUG");

        return res;
    }

    /*
    * Get string with TI register status
    */
    static const std::string ti_register_status(const uint32_t ti_state) {
        char buff[64];
        std::string res = "TI register: ";
        std::sprintf(buff, "0x%X\n", ti_state);
        res += buff;
        res += CoreCommon::bit_test(buff, ti_state, 0, "INTEN");
        res += CoreCommon::bit_test(buff, ti_state, 1, "TDMODE");
        res += CoreCommon::bit_test(buff, ti_state, 3, "WAIT_RESP");
        res += CoreCommon::bit_test(buff, ti_state, 4, "DEST_INC");
        res += CoreCommon::bit_test(buff, ti_state, 5, "DEST_WIDTH");
        res += CoreCommon::bit_test(buff, ti_state, 6, "DEST_DREQ");
        res += CoreCommon::bit_test(buff, ti_state, 7, "DEST_IGNORE");
        res += CoreCommon::bit_test(buff, ti_state, 8, "SRC_INC");
        res += CoreCommon::bit_test(buff, ti_state, 9, "SRC_WIDTH");
        res += CoreCommon::bit_test(buff, ti_state, 10, "SRC_DREQ");
        res += CoreCommon::bit_test(buff, ti_state, 11, "SRC_IGNORE");
        res += CoreCommon::bits_test(buff,  ((ti_state>>12)&0xF), "15-12 BURST_LENGTH");
        res += CoreCommon::bits_test(buff,  ((ti_state>>16)&0x1F), "20-16 PERMAP");
        res += CoreCommon::bits_test(buff,  ((ti_state>>21)&0x1F), "25-21 WAITS");
        res += CoreCommon::bit_test(buff, ti_state, 26, "NO_WIDE_BURSTS");

        return res;
    }


    /*
    * Get string with TI register status
    */
    static const std::string debug_register_status(const uint32_t debug_state) {
        char buff[64];
        std::string res = "DEBIG register: ";
        std::sprintf(buff, "0x%X\n", debug_state);
        res += buff;
        res += CoreCommon::bit_test(buff, debug_state, 0, "RLAST_NSET_ERROR");
        res += CoreCommon::bit_test(buff, debug_state, 0, "FIFO_ERROR");
        res += CoreCommon::bit_test(buff, debug_state, 0, "READ_ERROR");
        res += CoreCommon::bits_test(buff,  ((debug_state>>4)&0xf),  "7-4 OUTS_WRITES");
        res += CoreCommon::bits_test(buff,  ((debug_state>>8)&0x7f), "15-8 DMA_ID");
        res += CoreCommon::bits_test(buff,  ((debug_state>>16)&0xff), "24-16 DMA_STATE");
        res += CoreCommon::bits_test(buff,  ((debug_state>>25)&0x7),  "27-25 VERSION");
        res += CoreCommon::bit_test(buff, debug_state, 28, "LITE");

        return res;
    }

private:
    uintptr_t _addr_ctrl; //DMA control register address
    volatile dma_ctrl_regs* _dma_ctrl_regs;
};

/*
* DMA control class
*/
class DmaControl {
public:
    /*
    *
    */
    DmaControl(const uint16_t dma = 10) : _addr(0), _dma(dma), _started(false), _dma_regs(nullptr){
        assert(dma < 15);

        _addr = DmaInfo::dma_address(dma);

        std::cout <<  "DmaCtrl DMA channel: " << dma <<  " addr: 0x" << std::hex << _addr << std::endl;

        std::cout << "** DMA Control Condtructor " << std::endl;

        _dma_regs = piutils::map_memory<dma_regs>(_addr);
        if(_dma_regs == nullptr){
            logger::log(logger::LLOG::ERROR, "DmaCtrl", std::string(__func__) + " Fail to initialize DMA control");
            throw std::runtime_error(std::string("Fail to initialize DMA control"));
        }

        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " DMA channel: " + std::to_string(dma) + " addr: " + std::to_string(_addr));
    }

    /*
    *
    */
    virtual ~DmaControl(){
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " addr: " + std::to_string(_addr));

        std::cout << "** DMA Control Destructor " << std::endl;

        if(_dma_regs){

            logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " Check if busy");
            std::cout << " Check if busy" << std::endl;

            int i = 0;
            while( !is_finished() && ++i < 100){
                //TODO: Add counter?
            };


            if(i >= 100){
              std::cout << " Check if busy - FAILED" << std::endl;
              this->abort();
            }

            //reset DMA control state
            reset();

            std::cout << " Clear TI" << std::endl;
            _dma_regs->_ti = 0;
            usleep(100);

            //
            _dma_regs = piutils::unmap_memory<dma_regs>(static_cast<dma_regs*>((void*)_dma_regs));
        }
    }


    /*
    * Initialize connection
    */
    bool Initialize(const uint32_t cs_flags = cs_flags_pwm) {
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        std::cout << "************ DMA Control Initialize() **********" << std::endl;
        std::cout << DmaInfo::ti_register_status( _dma_regs->_ti) << std::endl;

        reset();

        set_cs_flags(cs_flags);

        return true;
    }

    /*
    *  Abort current operation
    */
    void abort(){
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));
        std::cout << "*** DMA Control abort() " << std::endl;

        _dma_regs->_cs |= DMA_REG_CS_ABORT;  //Abort current execution
        usleep(300);
    }

    /*
    * Reset DMA
    */
    void reset(){
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        std::cout << "*** DMA Control reset() " << std::endl;

        _dma_regs->_cs = DMA_REG_CS_RESET;  //Reset DMA
        usleep(300);
        //std::this_thread::sleep_for(std::chrono::microseconds(1000));

        //Clear Interrupt (INT) and DMA end (END) statuses
        _dma_regs->_cs = (DMA_REG_CS_INT | DMA_REG_CS_END);
        usleep(50);
        //std::this_thread::sleep_for(std::chrono::microseconds(10));

        //clear error flags
        _dma_regs->_debug = (DMA_REG_DEBUG_READ_ERROR | DMA_REG_DEBUG_FIFO_ERROR | DMA_REG_DEBUG_READ_LAST_NOT_SET_ERROR);
        usleep(50);
        //std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    /*
    * Start to process DMA Control block
    */
    bool process_control_block(const pi_core::core_dma::DmaControlBlock* dma_ctrl_blk) {
        logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__));

        std::cout << "process_control_block " << std::endl;

        //Check did we finis process the current DMA CB
        if( !is_finished() ){
            logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " DMA is still busy");

            std::cout << "process_control_block not finshed yet" << std::endl;
            return false;
        }

        std::cout << "process_control_block reset" << std::endl;
        reset();

        print_status(true);

        _started = true;

        std::cout << "process_control_block set DMS CB" << std::endl;
        std::cout << "process_control_block TI " << std::hex << DmaInfo::ti_register_status(dma_ctrl_blk->get_ctrl_blk()->_ti) << std::endl;

        //Set DMA Control Block Note: use physical address here!!!!
        _dma_regs->_conblk_ad = dma_ctrl_blk->get_phys_ctrl_blk();

        //Start to process DMA Control Block
        _dma_regs->_cs = DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES; // | DMA_REG_CS_DISDEBUG;
        //std::this_thread::sleep_for(std::chrono::microseconds(10));
        _dma_regs->_cs = (DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES | DMA_REG_CS_ACTIVE);
        //_dma_regs->_cs = DMA_REG_CS_ACTIVE;
        usleep(100);
        //std::this_thread::sleep_for(std::chrono::microseconds(100));

        print_status(true);

        return true;
   }

   void print_status(const bool fstatus = false) {
        const uint32_t txrf_len = _dma_regs->_txrf_len;
        std::cout << "--------- Length  " << std::dec <<  txrf_len << std::endl;
        if(fstatus){
          const uint32_t cs = _dma_regs->_cs;
          std::cout << "process_control_block -----> " << DmaInfo::cs_register_status(cs) << std::endl;
          const uint32_t debug_reg = _dma_regs->_debug;
          std::cout << "process_control_block -----> " << DmaInfo::debug_register_status(debug_reg) << std::endl;

        }
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
    * Check if we finished to process DMA CB
    * True - no more data
    * False - busy now
    */
    bool is_finished() {
        bool result = true;
        if(_started){
            result = ((_dma_regs->_cs & DMA_REG_CS_END) != 0);
            if(result){
                 _started = false;
                 logger::log(logger::LLOG::DEBUG, "DmaCtrl", std::string(__func__) + " DMA Control block was processed");
            }
            else{
                std::this_thread::sleep_for(std::chrono::microseconds(100));
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
        usleep(10);
    }

    static const uint32_t cs_flags_pwm = DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES | DMA_REG_CS_PANIC_PRIORITY(15) | DMA_REG_CS_PRIORITY(15);
    static const uint32_t cs_flags_test = DMA_REG_CS_WAIT_FOR_OUTSTANDING_WRITES;



private:
    volatile dma_regs* _dma_regs;
    uintptr_t _addr;       //DMA register address
    uint16_t _dma;   //DMA number
    uint32_t _cs_flags; //default flags for CS register
    bool _started;  //Flag do we have active Control Block on processing

};


}
}

#endif
