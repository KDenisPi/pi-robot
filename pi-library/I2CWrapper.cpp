/*
 * I2CWrapper.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: denis
 */

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


} /* namespace pirobot */
