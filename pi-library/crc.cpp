/*
 * crc.cpp
 *
 *  Computation of cyclic redundancy checks
 * 
 *  Created on: Feb 21, 2018
 *      Author: Denis Kudia
 */


#include "crc.h"

namespace pirobot {
namespace crc {

unsigned char crc(const unsigned char* data, const int dlen, const unsigned short init_val, const unsigned char polynomial){
  unsigned short res = init_val;
  int i, j;

  for(i = 0; i < dlen; i++){
    res = res ^ data[i];
    for(j = 0; j < 8; j++){
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
}
