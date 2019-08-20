/*
 * core_mailbox.h
 *
 * Maolbox access
 *
 *  Created on: July 9, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_CORE_PHYS_MAILBOX_ACCESS_H_
#define PI_CORE_PHYS_MAILBOX_ACCESS_H_

#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>

#include "mailbox.h"

#include "smallthings.h"
#include "core_common.h"

#define LLOG(text, value) std::cout << text << value << std::endl;
#define LLOGH(text, hvalue) std::cout << text << std::hex << hvalue << std::endl;

namespace pi_core {
namespace core_mailbox {

#define MAJOR_NUM 100
#define IOCTL_MBOX_PROPERTY _IOWR(MAJOR_NUM, 0, char *)

/*
Mailbox Read/Write Peek  Sender  Status    Config
   0    0x00       0x10  0x14    0x18      0x1c
   1    0x20       0x30  0x34    0x38      0x3c
 */
using mbox_regs = struct __attribute__((packed, aligned(4))) mbox_regs_t{
    uint32_t _read_write;  //Read/Write
    uint32_t _unused[3];
    uint32_t _peek;
    uint32_t _sender;
    uint32_t _status;
    uint32_t _config;
};

#define MEM_FLAG_DISCARDABLE    (1 << 0) /* can be resized to 0 at any time. Use for cached data */
#define MEM_FLAG_NORMAL         (0 << 2) /* normal allocating alias. Don't use from ARM */
#define MEM_FLAG_DIRECT         (1 << 2) /* 0xC alias uncached */
#define MEM_FLAG_COHERENT       (2 << 2) /* 0x8 alias. Non-allocating in L2 but coherent */
#define MEM_FLAG_L1_NONALLOCATING (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT) /* Allocating in L2 */
#define MEM_FLAG_ZERO           (1 << 4) /* initialise buffer to all zeros */
#define MEM_FLAG_NO_INIT        (1 << 5) /* don't initialise (default is initialise to all ones */
#define MEM_FLAG_HINT_PERMALOCK (1 << 6) /* Likely to be locked for long periods of time. */


#define BUS_TO_PHYS(x)                           ((x)&~0xC0000000)

class MailboxCore {
public:
    //base for maibox registers
    const uint32_t mailbox_base = 0xB880;

    const uint32_t mailbox_0_offset = 0x00;
    const uint32_t mailbox_1_offset = 0x20;

    const uint32_t maibox_request  = 0x00000000;
    const uint32_t maibox_response = 0x00000001;

    const uint32_t mailbox_full_flag = 0x80000000;
    const uint32_t mailbox_empty_flag = 0x40000000;

    const uint32_t tags_get_firmware = 0x00000001;

    /*

     */
    MailboxCore() : _mailbox(nullptr), _buffer(nullptr) {

        _sdram_addr = pi_core::CoreCommon::get_sdram_address();
        _peripheral_addr = pi_core::CoreCommon::get_peripheral_address();

        LLOGH("MailboxCore() SDRAM : ", _sdram_addr)
        LLOGH("MailboxCore() PERPH : ", _peripheral_addr)

        _fd = mbox_open();
    }

    /*

     */
    virtual ~MailboxCore() {
        mbox_close(_fd);
    }

/*
    * Allocation type
    * 1 - mmap, 2 - valloc, 3 - mailbox
    *
    *
    */
    const std::shared_ptr<pi_core::MemInfo>  get_memory(const size_t len){

        void* mem = nullptr; //virtual address
        uint32_t mbox_handle_alloc = 0; //allocated address
        uint32_t mbox_handle_lock = 0;  //bus address (physical address)

        mbox_handle_alloc = mem_alloc(_fd, len, getpagesize(), (_sdram_addr == 0x40000000 ? 0xC : 0x4));
        std::cout << "get_memory Hnd Alloc: " << std::hex << mbox_handle_alloc << std::endl;

        if(mbox_handle_alloc){
            mbox_handle_lock =  mem_lock(_fd, mbox_handle_alloc);
            std::cout << "get_memory Hnd Lock: " << std::hex << mbox_handle_alloc << std::endl;

            if(mbox_handle_lock){
                //virtual address
                mem = mapmem(mbox_handle_lock, len);
                std::cout << "get_memory Mapmem: " << std::hex << mbox_handle_alloc << std::endl;
            }
        }

        if(!mem){
            std::cout << "Mailbox mem_alloc Failed: " << std::endl;
            mem_unlock(_fd, mbox_handle_lock);
            mem_free(_fd, mbox_handle_alloc);
        }

        return std::shared_ptr<pi_core::MemInfo>(new pi_core::MemInfo(mem, mbox_handle_lock, len, 3, mbox_handle_alloc, mbox_handle_lock));
    }

        /*
    *
    */
    void free_memory(std::shared_ptr<pi_core::MemInfo>& minfo){
       std::cout << "deallocate_and_unlock_mailbox Bus address: " << std::hex << minfo->get_handle_lock() << " Len: " << std::hex << minfo->get_size() << std::endl;

        unmapmem(minfo->get_vaddr(), minfo->get_size());
        mem_unlock(_fd, minfo->get_handle_lock());
        mem_free(_fd, minfo->get_handle_alloc());

      return;
    }


private:

    int _fd;

    uint32_t* _mailbox; // mapped Maibox registers
    uint32_t* _buffer;   //request/response buffer

    uintptr_t _sdram_addr;
    uintptr_t _peripheral_addr;
};

}//namespace mailbox
}//namespace core

#endif
