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

#include "smallthings.h"
#include "core_common.h"
#include "mailbox.h"

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

/*
        _mailbox = piutils::map_memory<uint32_t>(_peripheral_addr + mailbox_base);
        //_mailbox = static_cast<uint32_t*>((void*)(_peripheral_addr + mailbox_base));
        LLOGH("MailboxCore() mailbox : ", _mailbox)

        _buffer = static_cast<uint32_t*>(aligned_alloc(1024, 24*sizeof(uint32_t)));
        if(!_buffer){
            LLOG("MailboxCore() Failed to allocate buffer: ", errno)
        }
        LLOGH("MailboxCore() buffer : ", _buffer)
*/
        _fd = mbox_open();
    }

    /*

     */
    virtual ~MailboxCore() {
/*
        if(_mailbox){
            _mailbox = piutils::unmap_memory<uint32_t>((uint32_t*)_mailbox);
        }

        if(_buffer)
            free(_buffer);
*/
        mbox_close(_fd);
    }

    /*
        Get firmvare version

        Tag: 0x00000001
            Request:
                Length: 0
            Response:
                Length: 4
                Value:
                    u32: firmware revision
     */

    uint32_t get_firmware_version(){
        /*

        int i = 0;
        _buffer[i++] = 0;
        _buffer[i++] = maibox_request;
        _buffer[i++] = tags_get_firmware;
        _buffer[i++] = 0; //length
        _buffer[i++] = 0; //code
        _buffer[i++] = 0; //end tag

        _buffer[0] = i*sizeof(uint32_t);

        bool result = mbox_property(_fd, (void*)_buffer);
        if(result){
            LLOGH(" get_firmware_version", _buffer[5]);
            return _buffer[5];
        }
*/
        unsigned mem_ref = mem_alloc(_fd, 1024, 1024, (_sdram_addr == 0x40000000 ? 0xC : 0x4));
        LLOGH(" Allocated ", mem_ref);
        if( mem_ref ){
            unsigned result = mem_free(_fd, mem_ref);
            LLOG(" **** Result ", result);
        }

        return 0;
    }

private:

/*
        return false;
        //check mailbox state
        mbox_regs* mbox_1 = reinterpret_cast<mbox_regs*>(_mailbox + mailbox_1_offset);
        mbox_regs* mbox_0 = reinterpret_cast<mbox_regs*>(_mailbox + mailbox_0_offset);
        std::cout << "Mailboxes: 0: " << std::hex << mbox_0 << " status: " << std::hex << (void*)&mbox_0->_status << std::endl;
        std::cout << "Mailboxes: 1: " << std::hex << mbox_1 << " status: " << std::hex << (void*)&mbox_1->_status << std::endl;

        //return false;

        while( (mbox_1->_status & mailbox_full_flag) != 0){
        }

        std::cout << "Request Length 0: 0x" << std::hex << data[0] << " Addr: " << std::hex << data << std::endl;
        //uint32_t request = 0x40000000 | ((uintptr_t)data << 4) | channel;
        //uint32_t request = 0x40000000 | ((uintptr_t)data << 4) | channel;
        uint32_t request = ((uintptr_t)data << 4) | channel;


        std::cout << "Request Length 1: 0x" << std::hex << data[0] << " Addr: 0x" << std::hex << request << std::endl;
        mbox_1->_read_write = request;
        std::cout << "Request Length 2: 0x" << std::hex << data[3] << " Code 0x" << std::hex << data[1] <<" Addr: 0x" << std::hex << request << std::endl;

        //return false;

        std::cout << "Start reading" << std::endl;
        for(;;){
           while( (mbox_0->_status & mailbox_empty_flag) != 0){

           }

           uint32_t response = mbox_0->_read_write;
           uint8_t r_channel = response & 0x0F;
           std::cout << "Response : 0x" << std::hex << response << " Channel: " << std::hex << r_channel << std::endl;

           if(channel == r_channel){
              response = (response >> 4);
              std::cout << "****** Response : 0x" << std::hex << response << std::endl;

              //uint32_t* r_data = static_cast<uint32_t*>((void*)response);
              //std::cout << "****** Response Length: 0x" << std::hex << r_data[0] << " Status: " << std::hex << r_data[1] << std::endl;
           }

           break;
        }

        //check response status
        return result;
    }
*/
    int _fd;

    uint32_t* _mailbox; // mapped Maibox registers
    uint32_t* _buffer;   //request/response buffer

    uintptr_t _sdram_addr;
    uintptr_t _peripheral_addr;
};

}//namespace mailbox
}//namespace core

#endif
