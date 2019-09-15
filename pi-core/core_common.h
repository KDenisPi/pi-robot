/*
 * core_common.h
 *
 * BCM2835 ARM Peripherals.
 *
 *  Created on: March 12, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_CORE_COMMON_H_
#define PI_CORE_COMMON_H_

#include "stdint.h"
#include <cstdio>

#ifdef REAL_HARDWARE
#include <bcm_host.h>
#endif

namespace pi_core {

/*
* The mappint of peripherals to DREQ (0-31)

    TODO: add another if needed
*/

enum DREQ {
    NO_required = 0,
    DSI = 1,
    PCM_TX = 2,
    PWM = 5,
    SPI_TX = 6
};

class CoreCommon {
public:
    /*
    * Get peripheral address
    */
    static const uintptr_t get_peripheral_address() {
#ifdef REAL_HARDWARE
        return bcm_host_get_peripheral_address();
#else
        return 0x3f000000;
#endif
    }

    static const uintptr_t get_sdram_address() {
#ifdef REAL_HARDWARE
        return bcm_host_get_sdram_address();
#else
        return 0xC0000000;
#endif
    }

    static const uint32_t get_frequency() {
        return 19200000;
    }

    static const char* bit_test(char * buff, const uint32_t value, uint8_t nbit, const char* name) {
        std::sprintf(buff, "%d %s: %d\n", nbit, name, ((((1<<nbit) & value) != 0) ? 1 : 0));
        return buff;
    }

    static const char* bits_test(char * buff, const uint32_t value, const char* name) {
        std::sprintf(buff, "%s: %d\n", name, value);
        return buff;
    }
};

/*
* First: memory map result
* Second: physical address for memory map result
*/
class MemInfo {
public:

    virtual ~MemInfo(){

    }

    /*
    *
    */
    size_t get_size() const {
        return _len;
    }

    void* get_vaddr() const {
        return _vmemory;
    }

    uintptr_t get_paddr() const {
        return _phmemory;
    }

    const bool is_empty() const {
        return (_len== 0);
    }

    int get_type() const {
        return _alloc_type;
    }

    uint32_t get_handle_alloc() const {
        return _mbox_handle_alloc;
    }

    uint32_t get_handle_lock() const {
        return _mbox_handle_lock;
    }

    MemInfo(void* vmemory, uintptr_t phmemory, size_t len, int alloc_type, uint32_t mbox_handle_alloc = 0, uint32_t mbox_handle_lock = 0) :
        _vmemory(vmemory), _phmemory(phmemory), _len(len), _alloc_type(alloc_type),
        _mbox_handle_alloc(mbox_handle_alloc),
        _mbox_handle_lock(mbox_handle_lock)
        {

        }

protected:
    void clear(){
        _vmemory = nullptr;
        _phmemory = 0L;
        _len = 0;
        _mbox_handle_alloc = 0;
        _mbox_handle_lock = 0;
    }

    void* _vmemory;         //virtual address
    uintptr_t _phmemory;    //physical address
    size_t _len;            //memory length
    int _alloc_type;        //type of allocation

    //fields used for mailbox allocation only
    uint32_t _mbox_handle_alloc = 0; //allocated address
    uint32_t _mbox_handle_lock = 0;  //bus address (physical address)
};

} //namespace pi_core

#endif
