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

} //namespace pi_core

#endif
