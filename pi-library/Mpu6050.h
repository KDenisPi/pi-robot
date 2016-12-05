/*
 * Mpu6050.h
 *
 *  Created on: Dec 2, 2016
 *      Author: denis
 */

// MPU-6050 Accelerometer + Gyro
// -----------------------------
//
// By arduino.cc user "Krodal".
// June 2012
// Open Source / Public Domain
// Modified by Debra - http://www.geekmomprojects.com/gyroscopes-and-accelerometers-on-a-chip/
//
// Using Arduino 1.0.1
// It will not work with an older version,
// since Wire.endTransmission() uses a parameter
// to hold or release the I2C bus.
//
// Documentation:
// - The InvenSense documents:
//   - "MPU-6000 and MPU-6050 Product Specification",
//     PS-MPU-6000A.pdf
//   - "MPU-6000 and MPU-6050 Register Map and Descriptions",
//     RM-MPU-6000A.pdf or RS-MPU-6000A.pdf
//   - "MPU-6000/MPU-6050 9-Axis Evaluation Board User Guide"
//     AN-MPU-6000EVB.pdf
//
// The accuracy is 16-bits.
//
// Temperature sensor from -40 to +85 degrees Celsius
//   340 per degrees, -512 at 35 degrees.
//
// At power-up, all registers are zero, except these two:
//      Register 0x6B (PWR_MGMT_2) = 0x40  (I read zero).
//      Register 0x75 (WHO_AM_I)   = 0x68.
//

// The name of the sensor is "MPU-6050".
// For program code, I omit the '-',
// therefor I use the name "MPU6050....".


#ifndef PI_LIBRARY_MPU6050_H_
#define PI_LIBRARY_MPU6050_H_

#include <mutex>

#include "Mpu6050_defs.h"
#include "Threaded.h"

namespace pirobot {
namespace mpu6050 {


// Declaring an union for the registers and the axis values.
// The byte order does not match the byte order of
// the compiler and AVR chip.
// The AVR chip (on the Arduino board) has the Low Byte
// at the lower address.
// But the MPU-6050 has a different order: High Byte at
// lower address, so that has to be corrected.
// The register part "reg" is only used internally,
// and are swapped in code.
union accel_t_gyro_union
{
  struct
  {
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  } reg;
  struct
  {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t temperature;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  } value;
};

struct mpu6050_values {
	unsigned long last_read_time;
	float         last_x_angle;  // These are the filtered angles
	float         last_y_angle;
	float         last_z_angle;
	float         last_gyro_x_angle;  // Store the gyro angles to compare drift
	float         last_gyro_y_angle;
	float         last_gyro_z_angle;
	float 		  last_temperature;
};


class Mpu6050 : public Threaded {
public:
	Mpu6050(const uint8_t i2caddr = MPU6050_I2C_ADDRESS, const unsigned int utime = 1000);
	virtual ~Mpu6050();

	void get_last_read_angle_data(struct mpu6050_values& values);

	inline unsigned long get_last_time() {return m_val.last_read_time;}
	inline float get_last_x_angle() {return m_val.last_x_angle;}
	inline float get_last_y_angle() {return m_val.last_y_angle;}
	inline float get_last_z_angle() {return m_val.last_z_angle;}
	inline float get_last_gyro_x_angle() {return m_val.last_gyro_x_angle;}
	inline float get_last_gyro_y_angle() {return m_val.last_gyro_y_angle;}
	inline float get_last_gyro_z_angle() {return m_val.last_gyro_z_angle;}

	// The sensor should be motionless on a horizontal surface
	//  while calibration is happening
	void calibrate_sensors();

	/*
	 * Update values - will be used in loop
	 */
	void update_values();

	virtual bool start();
	virtual const std::string to_string();

	static void* worker(void* p);
	virtual void stop();

	inline unsigned int get_utime() { return update_interval; }

private:
	uint8_t _i2caddr;
	int m_fd;

	unsigned int update_interval; //update interval (1 second by default)
	std::recursive_mutex data_update;

	// Use the following global variables and access functions to help store the overall
	// rotation angle of the sensor
	struct mpu6050_values m_val;

	void set_last_read_angle_data(unsigned long time, float x, float y, float z, float x_gyro, float y_gyro, float z_gyro, float temperature);
	int read_gyro_accel_vals(uint8_t* accel_t_gyro_ptr);

	//  Use the following global variables and access functions
	//  to calibrate the acceleration sensor
	float    base_x_accel;
	float    base_y_accel;
	float    base_z_accel;

	float    base_x_gyro;
	float    base_y_gyro;
	float    base_z_gyro;
};

} /* namespace mpu6050 */
} /* namespace pirobot */

#endif /* PI_LIBRARY_MPU6050_H_ */
