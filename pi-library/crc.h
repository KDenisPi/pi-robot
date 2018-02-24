
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

namespace pirobot {
namespace crc {

unsigned char crc(const unsigned char* data, const int dlen, const unsigned short init_val, const unsigned char polynomial);

} //crc
} //piutils

#endif
