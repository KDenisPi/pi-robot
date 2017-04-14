/*
 * I2CWrapper.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: Denis Kudia
 */

#include <cassert>
#include <cstring>

#include "I2CWrapper.h"

namespace pirobot {

std::recursive_mutex I2CWrapper::i2c;

int I2CWrapper::I2CRead(const int fd){
	return wiringPiI2CRead(fd);
}

int I2CWrapper::I2CReadReg8(const int fd, const int reg){
	return wiringPiI2CReadReg8(fd, reg);
}

int I2CWrapper::I2CReadReg16(const int fd, const int reg){
	return wiringPiI2CReadReg16(fd, reg);
}

int I2CWrapper::I2CWrite(const int fd, const int data){
	return wiringPiI2CWrite(fd, data);
}

int I2CWrapper::I2CWriteReg8(const int fd, const int reg, const int data){
	return wiringPiI2CWriteReg8(fd,  reg,  data);
}
int I2CWrapper::I2CWriteReg16(const int fd, const int reg, const int data){
	return wiringPiI2CWriteReg16(fd, reg,  data);
}

int I2CWrapper::I2CSetupInterface(const char *device, int devId){
	return wiringPiI2CSetupInterface(device, devId);
}

int I2CWrapper::I2CSetup(const int devId){
	return wiringPiI2CSetup(devId);
}

/*
 * Write block of data
 *
 * Note: Maximum size of block is 32 bytes
 */
int I2CWrapper::I2CWriteData(const int fd, const int reg, const uint8_t* buffer, const int size){
	i2c_smbus_data data;
	assert(size <= I2C_SMBUS_BLOCK_MAX);

	int rsize = (size > I2C_SMBUS_BLOCK_MAX ? I2C_SMBUS_BLOCK_MAX : size);

	data.block[0] = size;
	memcpy(&data.block[1], buffer, rsize);

	return I2CWrapper::i2c_smbus_access(fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BLOCK_DATA, &data);
}

/*
 * Read block of data
 *
 * TODO: I do not check output buffer there.
 * To be sure that buffer have enough length
 *
 */
int I2CWrapper::I2CReadData(const int fd, const int reg, uint8_t* buffer, const int size){
	i2c_smbus_data data;
	assert(size <= I2C_SMBUS_BLOCK_MAX);

	int rsize = (size > I2C_SMBUS_BLOCK_MAX ? I2C_SMBUS_BLOCK_MAX : size);
	memset(&data.block[0], 0x00, sizeof(data.block));
	data.block[0] = size;

	int result = I2CWrapper::i2c_smbus_access(fd, I2C_SMBUS_READ, reg, I2C_SMBUS_BLOCK_DATA, &data);
	if( result < 0 )
		return result;

	memcpy(buffer, &data.block[1], data.block[0]);
	return data.block[0];
}

} /* namespace pirobot */
