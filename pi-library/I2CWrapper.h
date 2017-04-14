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

#include <wiringPiI2C.h>

/*
 * I have detected that wiringPi does not implement data block read/write
 *
 * I have implemented this functionality by myself
 */

namespace pirobot {

// I2C definitions

#define I2C_SLAVE	0x0703
#define I2C_SMBUS	0x0720	/* SMBus-level access */

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
union i2c_smbus_data
{
  uint8_t  byte ;
  uint16_t word ;
  uint8_t  block [I2C_SMBUS_BLOCK_MAX + 2] ;	// block [0] is used for length + one more for PEC
} ;
#endif

#ifndef i2c_smbus_ioctl_data
struct i2c_smbus_ioctl_data
{
  char read_write ;
  uint8_t command ;
  int size ;
  union i2c_smbus_data *data ;
} ;
#endif

class I2CWrapper {
public:
	I2CWrapper() {}
	virtual ~I2CWrapper() {}

	static void lock() { i2c.lock(); }
	static void unlock() { i2c.unlock(); }

	static int I2CRead           (const int fd) ;
	static int I2CReadReg8       (const int fd, const int reg) ;
	static int I2CReadReg16      (const int fd, const int reg) ;

	static int I2CWriteData	(const int fd, const int reg, const uint8_t* buffer, const int size);
	static int I2CReadData	(const int fd, const int reg, uint8_t* buffer, const int size);

	static int I2CWrite          (const int fd, const int data) ;
	static int I2CWriteReg8      (const int fd, const int reg, const int data) ;
	static int I2CWriteReg16     (const int fd, const int reg, const int data) ;

	static int I2CSetupInterface (const char *device, int devId) ;
	static int I2CSetup          (const int devId) ;

	static inline int i2c_smbus_access (int fd, char rw, uint8_t command, int size, union i2c_smbus_data *data)
	{
	  struct i2c_smbus_ioctl_data args ;

	  args.read_write = rw ;
	  args.command    = command ;
	  args.size       = size ;
	  args.data       = data ;
	  return ioctl (fd, I2C_SMBUS, &args) ;
	}

private:
	static std::recursive_mutex i2c;

};

} /* namespace pirobot */

#endif /* PI_LIBRARY_I2CWRAPPER_H_ */
