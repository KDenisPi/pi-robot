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

namespace pi_core {

class CoreCommon {

public:
    /*
    * Get peripheral address
    */
    static const uint32_t get_peripheral_address() {
#ifdef REAL_HARDWARE
        return bcm_host_get_peripheral_address();
#else
        return 0x3f000000;
#endif
    }

    static const uint32_t get_frequency() {
        return 19200000;
    }

};

} //namespace pi_core

#endif
