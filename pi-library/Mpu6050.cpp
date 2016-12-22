/*
 * Mpu6050.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: denis
 */

#include <cstdio>
#include <cmath>

#include <wiringPi.h>

#include "Mpu6050.h"
#include "logger.h"
#include "I2CWrapper.h"

namespace pirobot {
namespace mpu6050 {

const char TAG[] = "MCP23017";

Mpu6050::Mpu6050(const uint8_t i2caddr, const unsigned int utime) :
	_i2caddr(i2caddr), m_fd(0), update_interval(utime),
	m_val({0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}),
	base_x_accel(0.0),
	base_y_accel(0.0),
	base_z_accel(0.0),
	base_x_gyro(0.0),
	base_y_gyro(0.0),
	base_z_gyro(0.0)
{
	  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Address: " + std::to_string(_i2caddr));
	  int error;
	  uint8_t model, sleep_mode;
	  char buff[10];

	  /*
	   * I do not want to ask bus to quickly
	   */
	  if(update_interval < 100)
		  update_interval = 100;

	  I2CWrapper::lock();
	  m_fd = I2CWrapper::I2CSetup(_i2caddr);

	  // default at power-up:
	  //    Gyro at 250 degrees second
	  //    Acceleration at 2g
	  //    Clock source at internal 8MHz
	  //    The device is in sleep mode.
	  //
	  model = I2CWrapper::I2CReadReg8(m_fd, MPU6050_WHO_AM_I);
	  std::sprintf(buff, "0x%X", model);

	  // According to the datasheet, the 'sleep' bit
	  // should read a '1'. But I read a '0'.
	  // That bit has to be cleared, since the sensor
	  // is in sleep mode at power-up. Even if the
	  // bit reads '0'.
	  sleep_mode = I2CWrapper::I2CReadReg8(m_fd, MPU6050_PWR_MGMT_2);

	  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " WHO_AM_I : " +
			  std::string(buff) + " Sleep mode : " + std::to_string(sleep_mode));

	  // Clear the 'sleep' bit to start the sensor.
	  I2CWrapper::I2CWriteReg8(m_fd, MPU6050_PWR_MGMT_1, 0);
	  I2CWrapper::unlock();

	  //Initialize the angles
	  calibrate_sensors();
	  set_last_read_angle_data(millis(), 0, 0, 0, 0, 0, 0, 0);

}

Mpu6050::~Mpu6050() {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Address: " + std::to_string(_i2caddr));
  stop();
}

/*
 *
 */
void Mpu6050::set_last_read_angle_data(unsigned long time,
		float x, float y, float z, float x_gyro, float y_gyro, float z_gyro, float temperature)
{
	data_update.lock();
	m_val.last_read_time = time;
	m_val.last_x_angle = x;
	m_val.last_y_angle = y;
	m_val.last_z_angle = z;
	m_val.last_gyro_x_angle = x_gyro;
	m_val.last_gyro_y_angle = y_gyro;
	m_val.last_gyro_z_angle = z_gyro;
	m_val.last_temperature = temperature;
	data_update.unlock();
}

void Mpu6050::get_last_read_angle_data(struct mpu6050_values& values){
	data_update.lock();
	values.last_read_time = m_val.last_read_time;
	values.last_x_angle = m_val.last_x_angle;
	values.last_y_angle = m_val.last_y_angle;
	values.last_z_angle = m_val.last_z_angle;
	values.last_gyro_x_angle= m_val.last_gyro_x_angle;
	values.last_gyro_y_angle = m_val.last_gyro_y_angle;
	values.last_gyro_z_angle = m_val.last_gyro_z_angle;
	values.last_temperature = m_val.last_temperature;
	data_update.unlock();
}
/*
 *
 */
int Mpu6050::read_gyro_accel_vals(uint8_t* accel_t_gyro_ptr){
	  // Read the raw values.
	  // Read 14 bytes at once,
	  // containing acceleration, temperature and gyro.
	  // With the default settings of the MPU-6050,
	  // there is no filter enabled, and the values
	  // are not very stable.  Returns the error value

	  accel_t_gyro_union* accel_t_gyro = (accel_t_gyro_union *) accel_t_gyro_ptr;

	  I2CWrapper::lock();
	  int error = I2CWrapper::I2CReadData(m_fd, MPU6050_ACCEL_XOUT_H, (uint8_t *) accel_t_gyro, sizeof(*accel_t_gyro));
	  I2CWrapper::unlock();

	  // Swap all high and low bytes.
	  // After this, the registers values are swapped,
	  // so the structure name like x_accel_l does no
	  // longer contain the lower byte.
	  uint8_t swap;
	  #define SWAP(x,y) swap = x; x = y; y = swap

	  SWAP ((*accel_t_gyro).reg.x_accel_h, (*accel_t_gyro).reg.x_accel_l);
	  SWAP ((*accel_t_gyro).reg.y_accel_h, (*accel_t_gyro).reg.y_accel_l);
	  SWAP ((*accel_t_gyro).reg.z_accel_h, (*accel_t_gyro).reg.z_accel_l);
	  SWAP ((*accel_t_gyro).reg.t_h, (*accel_t_gyro).reg.t_l);
	  SWAP ((*accel_t_gyro).reg.x_gyro_h, (*accel_t_gyro).reg.x_gyro_l);
	  SWAP ((*accel_t_gyro).reg.y_gyro_h, (*accel_t_gyro).reg.y_gyro_l);
	  SWAP ((*accel_t_gyro).reg.z_gyro_h, (*accel_t_gyro).reg.z_gyro_l);

	  return error;
}

// The sensor should be motionless on a horizontal surface
//  while calibration is happening
void Mpu6050::calibrate_sensors(){

  int                   num_readings = 10;
  float                 x_accel = 0;
  float                 y_accel = 0;
  float                 z_accel = 0;
  float                 x_gyro = 0;
  float                 y_gyro = 0;
  float                 z_gyro = 0;
  accel_t_gyro_union    accel_t_gyro;

  // Discard the first set of values read from the IMU
  read_gyro_accel_vals((uint8_t *) &accel_t_gyro);

  // Read and average the raw values from the IMU
  for (int i = 0; i < num_readings; i++) {

    read_gyro_accel_vals((uint8_t *) &accel_t_gyro);

    x_accel += accel_t_gyro.value.x_accel;
    y_accel += accel_t_gyro.value.y_accel;
    z_accel += accel_t_gyro.value.z_accel;
    x_gyro += accel_t_gyro.value.x_gyro;
    y_gyro += accel_t_gyro.value.y_gyro;
    z_gyro += accel_t_gyro.value.z_gyro;

    delay(100);
  }
  x_accel /= num_readings;
  y_accel /= num_readings;
  z_accel /= num_readings;
  x_gyro /= num_readings;
  y_gyro /= num_readings;
  z_gyro /= num_readings;

  // Store the raw calibration values globally
  base_x_accel = x_accel;
  base_y_accel = y_accel;
  base_z_accel = z_accel;
  base_x_gyro = x_gyro;
  base_y_gyro = y_gyro;
  base_z_gyro = z_gyro;
}

/*
 * Update values - will be used in loop
 */
void Mpu6050::update_values(){
	  int error;
	  double dT;
	  float temperature;
	  accel_t_gyro_union accel_t_gyro;

	  // Read the raw values.
	  error = read_gyro_accel_vals((uint8_t*) &accel_t_gyro);

	  // Get the time of reading for rotation computations
	  unsigned long t_now = millis();

	/*
	  Serial.print(F("Read accel, temp and gyro, error = "));
	  Serial.println(error,DEC);

	  // Print the raw acceleration values
	  Serial.print(F("accel x,y,z: "));
	  Serial.print(accel_t_gyro.value.x_accel, DEC);
	  Serial.print(F(", "));
	  Serial.print(accel_t_gyro.value.y_accel, DEC);
	  Serial.print(F(", "));
	  Serial.print(accel_t_gyro.value.z_accel, DEC);
	  Serial.println(F(""));
	*/

	  // The temperature sensor is -40 to +85 degrees Celsius.
	  // It is a signed integer.
	  // According to the datasheet:
	  //   340 per degrees Celsius, -512 at 35 degrees.
	  // At 0 degrees: -512 - (340 * 35) = -12412
	  temperature = ( (double) accel_t_gyro.value.temperature + 12412.0) / 340.0;
	/*
	  Serial.print(F("temperature: "));
	  dT = ( (double) accel_t_gyro.value.temperature + 12412.0) / 340.0;
	  Serial.print(dT, 3);
	  Serial.print(F(" degrees Celsius"));
	  Serial.println(F(""));

	  // Print the raw gyro values.
	  Serial.print(F("raw gyro x,y,z : "));
	  Serial.print(accel_t_gyro.value.x_gyro, DEC);
	  Serial.print(F(", "));
	  Serial.print(accel_t_gyro.value.y_gyro, DEC);
	  Serial.print(F(", "));
	  Serial.print(accel_t_gyro.value.z_gyro, DEC);
	  Serial.print(F(", "));
	  Serial.println(F(""));
	*/

	  // Convert gyro values to degrees/sec
	  float FS_SEL = 131;
	  float gyro_x = (accel_t_gyro.value.x_gyro - base_x_gyro)/FS_SEL;
	  float gyro_y = (accel_t_gyro.value.y_gyro - base_y_gyro)/FS_SEL;
	  float gyro_z = (accel_t_gyro.value.z_gyro - base_z_gyro)/FS_SEL;

	  // Get raw acceleration values
	  //float G_CONVERT = 16384;
	  float accel_x = accel_t_gyro.value.x_accel;
	  float accel_y = accel_t_gyro.value.y_accel;
	  float accel_z = accel_t_gyro.value.z_accel;

	  // Get angle values from accelerometer
	  float RADIANS_TO_DEGREES = 180/3.14159;
	  //float accel_vector_length = sqrt(pow(accel_x,2) + pow(accel_y,2) + pow(accel_z,2));
	  float accel_angle_y = atan(-1*accel_x/sqrt(pow(accel_y,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
	  float accel_angle_x = atan(accel_y/sqrt(pow(accel_x,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;

	  float accel_angle_z = atan(sqrt(pow(accel_x,2) + pow(accel_y,2))/accel_z)*RADIANS_TO_DEGREES;;
	  //float accel_angle_z = 0;

	  // Compute the (filtered) gyro angles
	  float dt =(t_now - get_last_time())/1000.0;
	  float gyro_angle_x = gyro_x*dt + get_last_x_angle();
	  float gyro_angle_y = gyro_y*dt + get_last_y_angle();
	  float gyro_angle_z = gyro_z*dt + get_last_z_angle();

	  // Compute the drifting gyro angles
	  float unfiltered_gyro_angle_x = gyro_x*dt + get_last_gyro_x_angle();
	  float unfiltered_gyro_angle_y = gyro_y*dt + get_last_gyro_y_angle();
	  float unfiltered_gyro_angle_z = gyro_z*dt + get_last_gyro_z_angle();

	  // Apply the complementary filter to figure out the change in angle - choice of alpha is
	  // estimated now.  Alpha depends on the sampling rate...
	  float alpha = 0.96;
	  float angle_x = alpha*gyro_angle_x + (1.0 - alpha)*accel_angle_x;
	  float angle_y = alpha*gyro_angle_y + (1.0 - alpha)*accel_angle_y;
	  float angle_z = gyro_angle_z;  //Accelerometer doesn't give z-angle

	  // Update the saved data with the latest values
	  set_last_read_angle_data(t_now, angle_x, angle_y, angle_z,
			  unfiltered_gyro_angle_x, unfiltered_gyro_angle_y, unfiltered_gyro_angle_z, temperature);
}

/*
* Prepare current values for output
*/
const std::string Mpu6050::print_current(){
  char buff[1024];
  struct mpu6050_values val;
  get_last_read_angle_data(val);

  std::sprintf(buff, "Time:%ld Angle [X:%0.3f Y:%0.3f Z:%0.3f] Gyro [X:%0.3f Y:%0.3f Z:%0.3f] Temp:%0.2f", 
	val.last_read_time,
	val.last_x_angle, val.last_y_angle, val.last_z_angle,
	val.last_gyro_x_angle, val.last_gyro_y_angle, val.last_gyro_z_angle, val.last_temperature);

  return std::string(buff);
}

/*
 *
 */
void Mpu6050::stop(){
	void* ret;
	int res = 0;

	set_stop_signal(true);
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(" Signal sent. Wait.. thread: ") + std::to_string(this->get_thread()));

	if( !is_stopped() ){
		res = pthread_join(this->get_thread(), &ret);
		if(res != 0)
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not join to thread Res:" + std::to_string(res));
	}

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished Res:" + std::to_string((long)ret));
}

/*
 *
 */
bool Mpu6050::start(void)
{
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started...");
	bool ret = true;

	if(is_stopped()){
		set_stop_signal(false);

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_t pthread;
		int result = pthread_create(&pthread, &attr, Mpu6050::worker, (void*)(this));
		if(result == 0){
			set_thread(pthread);
			logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Thread created");
		}
		else{
			//TODO: Exception
			logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Thread failed Res:" + std::to_string(result));
			ret = false;
		}
	}
	return ret;
}

/*
 *
 */
const std::string Mpu6050::to_string(){
	return "MPU6050 Over : " + std::to_string(_i2caddr);
}

/*
 *
 */
void* Mpu6050::worker(void* p){
	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker started.");

	Mpu6050* owner = static_cast<Mpu6050*>(p);
	while(!owner->is_stopSignal()){
		/*
		 *
		 */
		owner->update_values();

		delay(owner->get_utime());
	}

	logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished.");
	return (void*) 0L;
}


} /* namespace mpu6050 */
} /* namespace pirobot */
