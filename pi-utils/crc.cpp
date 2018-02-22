/*
 * crc.cpp
 *
 *  Computation of cyclic redundancy checks
 * 
 *  Created on: Feb 21, 2018
 *      Author: Denis Kudia
 */


#include "crc.h"

namespace crc {

uint8_t calculate_crc(const uint8_t* data, const int dlen, const uint16_t init_val, const uint8_t polynomial){
  uint16_t res = init_val;
  for(int i = 0; i < dlen; i++){
    res = res ^ data[i];
    for(int j = 0; j < 8; j++){
      if( (res & 0x80) ){
        res = (res << 1) ^ polynomial;
      } 
      else{
        res = (res << 1);
      }
    }
    res = res & 0x00FF;
  }

  return res;
}

}//crc
