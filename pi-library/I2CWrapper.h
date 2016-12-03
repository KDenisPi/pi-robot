/*
 * I2CWrapper.h
 *
 *  Created on: Dec 2, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_I2CWRAPPER_H_
#define PI_LIBRARY_I2CWRAPPER_H_

#include <mutex>
#include <wiringPiI2C.h>

namespace pirobot {

class I2CWrapper {
public:
	I2CWrapper() {}
	virtual ~I2CWrapper() {}

	static void lock() { i2c.lock(); }
	static void unlock() { i2c.unlock(); }

	static int I2CRead           (const int fd) ;
	static int I2CReadReg8       (const int fd, const int reg) ;
	static int I2CReadReg16      (const int fd, const int reg) ;

	static int I2CWrite          (const int fd, const int data) ;
	static int I2CWriteReg8      (const int fd, const int reg, const int data) ;
	static int I2CWriteReg16     (const int fd, const int reg, const int data) ;

	static int I2CSetupInterface (const char *device, int devId) ;
	static int I2CSetup          (const int devId) ;


private:
	static std::recursive_mutex i2c;

};

} /* namespace pirobot */

#endif /* PI_LIBRARY_I2CWRAPPER_H_ */
