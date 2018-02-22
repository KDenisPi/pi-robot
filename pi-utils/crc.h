
/*
 * crc.h
 *
 *  Computation of cyclic redundancy checks
 * 
 *  Created on: Feb 21, 2018
 *      Author: Denis Kudia
 */

#include "stdint.h"

#ifndef PI_UTILS_CRC_H
#define PI_UTILS_CRC_H

namespace crc {
uint8_t calculate_crc(const uint8_t* data, const int dlen, const uint16_t init_val, const uint8_t polynomial);

} //crc

#endif