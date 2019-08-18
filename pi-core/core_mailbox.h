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

        _fd = mbox_open();
    }

    /*

     */
    virtual ~MailboxCore() {
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
        return 0;
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
