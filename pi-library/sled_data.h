/*
 * sled_data.h
 *
 * LED Stripe Controller. Map for 1 ti 3 bit conversion
 *
 *  Created on: Dec 15, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SLED_DATA_CTRL_H_
#define PI_LIBRARY_SLED_DATA_CTRL_H_

namespace pirobot {
namespace sledctrl {

const std::uint8_t sled_data[] = {
0b10010010, 0b01001001, 0b00100100, 0b10010010, 0b01001001, 0b00100110,
0b10010010, 0b01001001, 0b00110100, 0b10010010, 0b01001001, 0b00110110,
0b10010010, 0b01001001, 0b10100100, 0b10010010, 0b01001001, 0b10100110,
0b10010010, 0b01001001, 0b10110100, 0b10010010, 0b01001001, 0b10110110,
0b10010010, 0b01001101, 0b00100100, 0b10010010, 0b01001101, 0b00100110,
0b10010010, 0b01001101, 0b00110100, 0b10010010, 0b01001101, 0b00110110,
0b10010010, 0b01001101, 0b10100100, 0b10010010, 0b01001101, 0b10100110,
0b10010010, 0b01001101, 0b10110100, 0b10010010, 0b01001101, 0b10110110,
0b10010010, 0b01101001, 0b00100100, 0b10010010, 0b01101001, 0b00100110,
0b10010010, 0b01101001, 0b00110100, 0b10010010, 0b01101001, 0b00110110,
0b10010010, 0b01101001, 0b10100100, 0b10010010, 0b01101001, 0b10100110,
0b10010010, 0b01101001, 0b10110100, 0b10010010, 0b01101001, 0b10110110,
0b10010010, 0b01101101, 0b00100100, 0b10010010, 0b01101101, 0b00100110,
0b10010010, 0b01101101, 0b00110100, 0b10010010, 0b01101101, 0b00110110,
0b10010010, 0b01101101, 0b10100100, 0b10010010, 0b01101101, 0b10100110,
0b10010010, 0b01101101, 0b10110100, 0b10010010, 0b01101101, 0b10110110,
0b10010011, 0b01001001, 0b00100100, 0b10010011, 0b01001001, 0b00100110,
0b10010011, 0b01001001, 0b00110100, 0b10010011, 0b01001001, 0b00110110,
0b10010011, 0b01001001, 0b10100100, 0b10010011, 0b01001001, 0b10100110,
0b10010011, 0b01001001, 0b10110100, 0b10010011, 0b01001001, 0b10110110,
0b10010011, 0b01001101, 0b00100100, 0b10010011, 0b01001101, 0b00100110,
0b10010011, 0b01001101, 0b00110100, 0b10010011, 0b01001101, 0b00110110,
0b10010011, 0b01001101, 0b10100100, 0b10010011, 0b01001101, 0b10100110,
0b10010011, 0b01001101, 0b10110100, 0b10010011, 0b01001101, 0b10110110,
0b10010011, 0b01101001, 0b00100100, 0b10010011, 0b01101001, 0b00100110,
0b10010011, 0b01101001, 0b00110100, 0b10010011, 0b01101001, 0b00110110,
0b10010011, 0b01101001, 0b10100100, 0b10010011, 0b01101001, 0b10100110,
0b10010011, 0b01101001, 0b10110100, 0b10010011, 0b01101001, 0b10110110,
0b10010011, 0b01101101, 0b00100100, 0b10010011, 0b01101101, 0b00100110,
0b10010011, 0b01101101, 0b00110100, 0b10010011, 0b01101101, 0b00110110,
0b10010011, 0b01101101, 0b10100100, 0b10010011, 0b01101101, 0b10100110,
0b10010011, 0b01101101, 0b10110100, 0b10010011, 0b01101101, 0b10110110,
0b10011010, 0b01001001, 0b00100100, 0b10011010, 0b01001001, 0b00100110,
0b10011010, 0b01001001, 0b00110100, 0b10011010, 0b01001001, 0b00110110,
0b10011010, 0b01001001, 0b10100100, 0b10011010, 0b01001001, 0b10100110,
0b10011010, 0b01001001, 0b10110100, 0b10011010, 0b01001001, 0b10110110,
0b10011010, 0b01001101, 0b00100100, 0b10011010, 0b01001101, 0b00100110,
0b10011010, 0b01001101, 0b00110100, 0b10011010, 0b01001101, 0b00110110,
0b10011010, 0b01001101, 0b10100100, 0b10011010, 0b01001101, 0b10100110,
0b10011010, 0b01001101, 0b10110100, 0b10011010, 0b01001101, 0b10110110,
0b10011010, 0b01101001, 0b00100100, 0b10011010, 0b01101001, 0b00100110,
0b10011010, 0b01101001, 0b00110100, 0b10011010, 0b01101001, 0b00110110,
0b10011010, 0b01101001, 0b10100100, 0b10011010, 0b01101001, 0b10100110,
0b10011010, 0b01101001, 0b10110100, 0b10011010, 0b01101001, 0b10110110,
0b10011010, 0b01101101, 0b00100100, 0b10011010, 0b01101101, 0b00100110,
0b10011010, 0b01101101, 0b00110100, 0b10011010, 0b01101101, 0b00110110,
0b10011010, 0b01101101, 0b10100100, 0b10011010, 0b01101101, 0b10100110,
0b10011010, 0b01101101, 0b10110100, 0b10011010, 0b01101101, 0b10110110,
0b10011011, 0b01001001, 0b00100100, 0b10011011, 0b01001001, 0b00100110,
0b10011011, 0b01001001, 0b00110100, 0b10011011, 0b01001001, 0b00110110,
0b10011011, 0b01001001, 0b10100100, 0b10011011, 0b01001001, 0b10100110,
0b10011011, 0b01001001, 0b10110100, 0b10011011, 0b01001001, 0b10110110,
0b10011011, 0b01001101, 0b00100100, 0b10011011, 0b01001101, 0b00100110,
0b10011011, 0b01001101, 0b00110100, 0b10011011, 0b01001101, 0b00110110,
0b10011011, 0b01001101, 0b10100100, 0b10011011, 0b01001101, 0b10100110,
0b10011011, 0b01001101, 0b10110100, 0b10011011, 0b01001101, 0b10110110,
0b10011011, 0b01101001, 0b00100100, 0b10011011, 0b01101001, 0b00100110,
0b10011011, 0b01101001, 0b00110100, 0b10011011, 0b01101001, 0b00110110,
0b10011011, 0b01101001, 0b10100100, 0b10011011, 0b01101001, 0b10100110,
0b10011011, 0b01101001, 0b10110100, 0b10011011, 0b01101001, 0b10110110,
0b10011011, 0b01101101, 0b00100100, 0b10011011, 0b01101101, 0b00100110,
0b10011011, 0b01101101, 0b00110100, 0b10011011, 0b01101101, 0b00110110,
0b10011011, 0b01101101, 0b10100100, 0b10011011, 0b01101101, 0b10100110,
0b10011011, 0b01101101, 0b10110100, 0b10011011, 0b01101101, 0b10110110,
0b11010010, 0b01001001, 0b00100100, 0b11010010, 0b01001001, 0b00100110,
0b11010010, 0b01001001, 0b00110100, 0b11010010, 0b01001001, 0b00110110,
0b11010010, 0b01001001, 0b10100100, 0b11010010, 0b01001001, 0b10100110,
0b11010010, 0b01001001, 0b10110100, 0b11010010, 0b01001001, 0b10110110,
0b11010010, 0b01001101, 0b00100100, 0b11010010, 0b01001101, 0b00100110,
0b11010010, 0b01001101, 0b00110100, 0b11010010, 0b01001101, 0b00110110,
0b11010010, 0b01001101, 0b10100100, 0b11010010, 0b01001101, 0b10100110,
0b11010010, 0b01001101, 0b10110100, 0b11010010, 0b01001101, 0b10110110,
0b11010010, 0b01101001, 0b00100100, 0b11010010, 0b01101001, 0b00100110,
0b11010010, 0b01101001, 0b00110100, 0b11010010, 0b01101001, 0b00110110,
0b11010010, 0b01101001, 0b10100100, 0b11010010, 0b01101001, 0b10100110,
0b11010010, 0b01101001, 0b10110100, 0b11010010, 0b01101001, 0b10110110,
0b11010010, 0b01101101, 0b00100100, 0b11010010, 0b01101101, 0b00100110,
0b11010010, 0b01101101, 0b00110100, 0b11010010, 0b01101101, 0b00110110,
0b11010010, 0b01101101, 0b10100100, 0b11010010, 0b01101101, 0b10100110,
0b11010010, 0b01101101, 0b10110100, 0b11010010, 0b01101101, 0b10110110,
0b11010011, 0b01001001, 0b00100100, 0b11010011, 0b01001001, 0b00100110,
0b11010011, 0b01001001, 0b00110100, 0b11010011, 0b01001001, 0b00110110,
0b11010011, 0b01001001, 0b10100100, 0b11010011, 0b01001001, 0b10100110,
0b11010011, 0b01001001, 0b10110100, 0b11010011, 0b01001001, 0b10110110,
0b11010011, 0b01001101, 0b00100100, 0b11010011, 0b01001101, 0b00100110,
0b11010011, 0b01001101, 0b00110100, 0b11010011, 0b01001101, 0b00110110,
0b11010011, 0b01001101, 0b10100100, 0b11010011, 0b01001101, 0b10100110,
0b11010011, 0b01001101, 0b10110100, 0b11010011, 0b01001101, 0b10110110,
0b11010011, 0b01101001, 0b00100100, 0b11010011, 0b01101001, 0b00100110,
0b11010011, 0b01101001, 0b00110100, 0b11010011, 0b01101001, 0b00110110,
0b11010011, 0b01101001, 0b10100100, 0b11010011, 0b01101001, 0b10100110,
0b11010011, 0b01101001, 0b10110100, 0b11010011, 0b01101001, 0b10110110,
0b11010011, 0b01101101, 0b00100100, 0b11010011, 0b01101101, 0b00100110,
0b11010011, 0b01101101, 0b00110100, 0b11010011, 0b01101101, 0b00110110,
0b11010011, 0b01101101, 0b10100100, 0b11010011, 0b01101101, 0b10100110,
0b11010011, 0b01101101, 0b10110100, 0b11010011, 0b01101101, 0b10110110,
0b11011010, 0b01001001, 0b00100100, 0b11011010, 0b01001001, 0b00100110,
0b11011010, 0b01001001, 0b00110100, 0b11011010, 0b01001001, 0b00110110,
0b11011010, 0b01001001, 0b10100100, 0b11011010, 0b01001001, 0b10100110,
0b11011010, 0b01001001, 0b10110100, 0b11011010, 0b01001001, 0b10110110,
0b11011010, 0b01001101, 0b00100100, 0b11011010, 0b01001101, 0b00100110,
0b11011010, 0b01001101, 0b00110100, 0b11011010, 0b01001101, 0b00110110,
0b11011010, 0b01001101, 0b10100100, 0b11011010, 0b01001101, 0b10100110,
0b11011010, 0b01001101, 0b10110100, 0b11011010, 0b01001101, 0b10110110,
0b11011010, 0b01101001, 0b00100100, 0b11011010, 0b01101001, 0b00100110,
0b11011010, 0b01101001, 0b00110100, 0b11011010, 0b01101001, 0b00110110,
0b11011010, 0b01101001, 0b10100100, 0b11011010, 0b01101001, 0b10100110,
0b11011010, 0b01101001, 0b10110100, 0b11011010, 0b01101001, 0b10110110,
0b11011010, 0b01101101, 0b00100100, 0b11011010, 0b01101101, 0b00100110,
0b11011010, 0b01101101, 0b00110100, 0b11011010, 0b01101101, 0b00110110,
0b11011010, 0b01101101, 0b10100100, 0b11011010, 0b01101101, 0b10100110,
0b11011010, 0b01101101, 0b10110100, 0b11011010, 0b01101101, 0b10110110,
0b11011011, 0b01001001, 0b00100100, 0b11011011, 0b01001001, 0b00100110,
0b11011011, 0b01001001, 0b00110100, 0b11011011, 0b01001001, 0b00110110,
0b11011011, 0b01001001, 0b10100100, 0b11011011, 0b01001001, 0b10100110,
0b11011011, 0b01001001, 0b10110100, 0b11011011, 0b01001001, 0b10110110,
0b11011011, 0b01001101, 0b00100100, 0b11011011, 0b01001101, 0b00100110,
0b11011011, 0b01001101, 0b00110100, 0b11011011, 0b01001101, 0b00110110,
0b11011011, 0b01001101, 0b10100100, 0b11011011, 0b01001101, 0b10100110,
0b11011011, 0b01001101, 0b10110100, 0b11011011, 0b01001101, 0b10110110,
0b11011011, 0b01101001, 0b00100100, 0b11011011, 0b01101001, 0b00100110,
0b11011011, 0b01101001, 0b00110100, 0b11011011, 0b01101001, 0b00110110,
0b11011011, 0b01101001, 0b10100100, 0b11011011, 0b01101001, 0b10100110,
0b11011011, 0b01101001, 0b10110100, 0b11011011, 0b01101001, 0b10110110,
0b11011011, 0b01101101, 0b00100100, 0b11011011, 0b01101101, 0b00100110,
0b11011011, 0b01101101, 0b00110100, 0b11011011, 0b01101101, 0b00110110,
0b11011011, 0b01101101, 0b10100100, 0b11011011, 0b01101101, 0b10100110,
0b11011011, 0b01101101, 0b10110100, 0b11011011, 0b01101101, 0b10110110
};

}
}

#endif