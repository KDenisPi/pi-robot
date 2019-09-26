/*
 * I2CWrapper.h
 *
 *  Created on: Dec 2, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_I2CWRAPPER_H_
#define PI_LIBRARY_I2CWRAPPER_H_

#include <fcntl.h>
#include <sys/ioctl.h>
#include <mutex>

#include "logger.h"

/*
 * I have detected that wiringPi does not implement data block read/write
 *
 * I have implemented this functionality by myself (but it seems it does not work)
 */

namespace pirobot {
namespace i2c {

// I2C definitions

//
// from linux/i2c-dev.h
//
/* /dev/i2c-X ioctl commands.  The ioctl's parameter is always an
 * unsigned long, except for:
 *      - I2C_FUNCS, takes pointer to an unsigned long
 *      - I2C_RDWR, takes pointer to struct i2c_rdwr_ioctl_data
 *      - I2C_SMBUS, takes pointer to struct i2c_smbus_ioctl_data
 */
#define I2C_RETRIES     0x0701  /* number of times a device address should
                                   be polled when not acknowledging */
#define I2C_TIMEOUT     0x0702  /* set timeout in units of 10 ms */

/* NOTE: Slave address is 7 or 10 bits, but 10-bit addresses
 * are NOT supported! (due to code brokenness)
 */
#define I2C_SLAVE       0x0703  /* Use this slave address */
#define I2C_SLAVE_FORCE 0x0706  /* Use this slave address, even if it
                                   is already in use by a driver! */
#define I2C_TENBIT      0x0704  /* 0 for 7 bit addrs, != 0 for 10 bit */

#define I2C_FUNCS       0x0705  /* Get the adapter functionality mask */

#define I2C_RDWR        0x0707  /* Combined R/W transfer (one STOP only) */

#define I2C_PEC         0x0708  /* != 0 to use PEC with SMBus */
#define I2C_SMBUS       0x0720  /* SMBus transfer */


#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

// SMBus transaction types

#define I2C_SMBUS_QUICK		    0
#define I2C_SMBUS_BYTE		    1
#define I2C_SMBUS_BYTE_DATA	    2
#define I2C_SMBUS_WORD_DATA	    3
#define I2C_SMBUS_PROC_CALL	    4
#define I2C_SMBUS_BLOCK_DATA	    5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7		/* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

// SMBus messages

#define I2C_SMBUS_BLOCK_MAX	32	/* As specified in SMBus standard */
#define I2C_SMBUS_I2C_BLOCK_MAX	32	/* Not specified but we use same structure */

// Structures used in the ioctl() calls

#ifndef i2c_smbus_data
using i2c_data = union i2c_smbus_data
{
  uint8_t  byte ;
  uint16_t word ;
  uint8_t  block [I2C_SMBUS_BLOCK_MAX + 2] ;	// block [0] is used for length + one more for PEC
} ;
#endif

#ifndef i2c_smbus_ioctl_data
using i2c_ioctl_data = struct i2c_smbus_ioctl_data
{
  char read_write ;
  uint8_t command ;
  int size ;
  union i2c_smbus_data *data ;
} ;
#endif

class I2CWrapper {
public:

    /*
    *
    */
    I2CWrapper(const std::string dname = "/dev/i2c-1") : _device(dname) {
        logger::log(logger::LLOG::INFO, "I2C", std::string(__func__) + " Device name: " + _device);
    }

    virtual ~I2CWrapper() {
        logger::log(logger::LLOG::INFO, "I2C", std::string(__func__) + " Device name: " + _device);
    }

    int I2CRead(const int fd) ;
    int I2CReadReg8(const int fd, const int reg) ;
    int I2CReadReg16(const int fd, const int reg) ;
    int I2CWriteData(const int fd, const int reg, const uint8_t* buffer, const int size);
    int I2CReadData(const int fd, const int reg, uint8_t* buffer, const int size);
    int I2CWrite(const int fd, const int data) ;
    int I2CWriteReg8(const int fd, const int reg, const int data) ;
    int I2CWriteReg16(const int fd, const int reg, const int data) ;

    //Initialize I2C interface for slave with address
    int I2CSetup(const int slave_addr) noexcept(false);
    void I2CClose(const int fd);

private:
    /*
    *
    */
    inline int i2c_smbus_access (int fd, char rw, uint8_t command, int size, union i2c_smbus_data *data)
    {
        struct i2c_smbus_ioctl_data args ;

        args.read_write = rw ;
        args.command    = command ;
        args.size       = size ;
        args.data       = data ;

        std::lock_guard<std::mutex> lock(_i2c);
        return ioctl (fd, I2C_SMBUS, &args) ;
    }

    std::mutex _i2c;
    std::string _device;
};

}//namespace i2c
} /* namespace pirobot */

#endif /* PI_LIBRARY_I2CWRAPPER_H_ */
