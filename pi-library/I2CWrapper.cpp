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
namespace i2c{

const char TAG[] = "I2C";

/*
* General read byte from I2C
*/
int I2CWrapper::I2CRead(const int fd){
    i2c_data data;
    int err = i2c_smbus_access(fd, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data);
    if(err < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + std::string(" Errno: ") + std::to_string(errno));
        return -1;
    }
    return (data.byte & 0xFF);
}

/*
* Read byte from register
*/
int I2CWrapper::I2CReadReg8(const int fd, const int reg){
    i2c_data data;
    int err = i2c_smbus_access(fd, I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data);
    if(err < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + std::string(" Errno: ") + std::to_string(errno));
        return -1;
    }
    return (data.byte & 0xFF);
}

/*
* Read word from register
*/
int I2CWrapper::I2CReadReg16(const int fd, const int reg){
    i2c_data data;
    int err = i2c_smbus_access(fd, I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data);
    if(err < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + std::string(" Errno: ") + std::to_string(errno));
        return -1;
    }
    return (data.word & 0xFFFF);
}

/*
* General write to I2C
*/
int I2CWrapper::I2CWrite(const int fd, const int wdata){
    int err = i2c_smbus_access(fd, I2C_SMBUS_WRITE, wdata, I2C_SMBUS_WORD_DATA, NULL);
    if(err < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + std::string(" Errno: ") + std::to_string(errno));
    }
    return err;
}

/*
* Write byte to register
*/
int I2CWrapper::I2CWriteReg8(const int fd, const int reg, const int wdata){
  i2c_data data ;

  data.byte = wdata ;
  int err = i2c_smbus_access (fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data) ;
    if(err < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + std::string(" Errno: ") + std::to_string(errno));
    }
    return err;
}

/*
* Write word to register
*/
int I2CWrapper::I2CWriteReg16(const int fd, const int reg, const int wdata){
  i2c_data data ;

  data.word = wdata ;
  int err = i2c_smbus_access (fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data) ;
    if(err < 0){
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + std::string(" Errno: ") + std::to_string(errno));
    }
    return err;
}

/*
* Initialize connecton with slave using slave address
*/
int I2CWrapper::I2CSetup(const int slave_addr){
    logger::log(logger::LLOG::INFO, TAG, std::string(__func__) + std::string(" Slave Addres: ") + std::to_string(slave_addr));

    int fd;
    std::lock_guard<std::mutex> lock(_i2c);

    //open device
    fd = open (_device.c_str(), O_RDWR);
    if(fd < 0){
        const std::string err = std::string(" Could not initialize device. errno: ") + std::to_string(errno);
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + err);
        throw std::runtime_error(err);
    }

    //connect slave device
    if(ioctl(fd, I2C_SLAVE, slave_addr) < 0){
        close(fd);
        const std::string err = std::string(" Could not initialize slave device errno: ") + std::to_string(errno);
        logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + err);
        throw std::runtime_error(err);
    }

    return fd;
}

void I2CWrapper::I2CClose(const int fd){
    if(fd >0)
        close(fd);
}


/*
 * Write block of data
 *
 * Note: Maximum size of block is 32 bytes
 */
int I2CWrapper::I2CWriteData(const int fd, const int reg, const uint8_t* buffer, const int size){
    i2c_data data;
    assert(size <= I2C_SMBUS_BLOCK_MAX);

    int rsize = (size > I2C_SMBUS_BLOCK_MAX ? I2C_SMBUS_BLOCK_MAX : size);

    data.block[0] = size;
    memcpy(&data.block[1], buffer, rsize);

    return i2c_smbus_access(fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_I2C_BLOCK_DATA, &data);
}

/*
 * Read block of data
 *
 * TODO: I do not check output buffer there.
 * To be sure that buffer have enough length
 *
 */
int I2CWrapper::I2CReadData(const int fd, const int reg, uint8_t* buffer, const int size){
    i2c_data data;
    assert(size <= I2C_SMBUS_BLOCK_MAX);

    int rsize = (size > I2C_SMBUS_BLOCK_MAX ? I2C_SMBUS_BLOCK_MAX : size);
    memset(&data.block[0], 0x00, sizeof(data.block));
    data.block[0] = size;

    int result = i2c_smbus_access(fd, I2C_SMBUS_READ, reg, I2C_SMBUS_I2C_BLOCK_DATA, &data);
    if( result < 0 )
        return result;

    memcpy(buffer, &data.block[1], data.block[0]);
    return data.block[0];
}

}// namespace i2c
} /* namespace pirobot */
