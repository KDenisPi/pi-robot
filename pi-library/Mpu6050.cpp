/*
 * Mpu6050.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: Denis Kudia
 */

#include <cstdio>
#include <cmath>
#include <iostream>

#include <wiringPi.h>

#include "Mpu6050.h"
#include "logger.h"
#include "I2CWrapper.h"

namespace pirobot {
namespace mpu6050 {

const char TAG[] = "MCP23017";

float Mpu6050::accel_LSB_Sensitivity[] = {16384, 8192, 4096, 2048};
float Mpu6050::gyro_LSB_Sensitivity[] = {131.0, 65.5, 32.8, 16.4};

Mpu6050::Mpu6050(const std::string& name,
    const std::shared_ptr<pirobot::i2c::I2C> i2c,
    const std::string& comment,
    const uint8_t i2caddr, 
    const unsigned int loop_delay) :
    item::Item(name, comment, item::ItemTypes::MPU6050),
    _i2caddr(i2caddr), m_fd(0),
    m_val({0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}),
    base_x_accel(0.0),
    base_y_accel(0.0),
    base_z_accel(0.0),
    base_x_gyro(0.0),
    base_y_gyro(0.0),
    base_z_gyro(0.0),
    m_gyro_conf(0), m_accel_conf(0)
{
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Address: " + std::to_string(_i2caddr));

    set_loop_delay(loop_delay);
    
    int error;
    uint8_t model, value;
    char buff[10];

    //register I2C user
    i2c->add_user(name, _i2caddr);

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

    initialize();

    m_gyro_conf = gyro_get_full_scale_range();
    m_accel_conf = accel_get_full_scale_range();

    I2CWrapper::unlock();

    m_sleep_mode = get_sleep();
    if(m_sleep_mode)
      set_sleep(false);

    m_sleep_mode = get_sleep();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " WHO_AM_I : " + std::string(buff) + " Sleep mode : " + std::to_string(m_sleep_mode));

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " FS_SEL : " +
            std::to_string(m_gyro_conf) + " AFS_SEL : " + std::to_string(m_accel_conf));

    //Initialize the angles
    calibrate_sensors();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
}

Mpu6050::~Mpu6050() {
  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Address: " + std::to_string(_i2caddr));
  stop();
}

bool Mpu6050::initialize(){
    I2CWrapper::I2CWriteReg8(m_fd, MPU6050_USER_CTRL, 0x07);
    delay(200);
    return true;
}

void Mpu6050::gyro_set_full_scale_range(int range){
    I2CWrapper::lock();
    uint8_t value = I2CWrapper::I2CReadReg8(m_fd, MPU6050_GYRO_CONFIG);

    switch(range){
      case 0:
        value &= 0xE7;
      case 1:
      case 2:
        value = SET_BIT(value, range);
      case 3:
        value |= 0x18;
     default:
        return;
    }

    I2CWrapper::I2CWriteReg8(m_fd, MPU6050_GYRO_CONFIG, value);
    I2CWrapper::unlock();

    m_gyro_conf = range;
}

int Mpu6050::gyro_get_full_scale_range(){
    I2CWrapper::lock();
    uint8_t value = I2CWrapper::I2CReadReg8(m_fd, MPU6050_GYRO_CONFIG);
    I2CWrapper::unlock();

    return (value & 0x18) >> 3;
}

void Mpu6050::accel_set_full_scale_range(int range){

    I2CWrapper::lock();
    uint8_t value = I2CWrapper::I2CReadReg8(m_fd, MPU6050_ACCEL_CONFIG);

    switch(range){
      case 0:
        value &= 0xE7;
      case 1:
      case 2:
        value = SET_BIT(value, range);
      case 3:
        value |= 0x18;
     default:
        return;
    }
    
    I2CWrapper::I2CWriteReg8(m_fd, MPU6050_ACCEL_CONFIG, value);
    I2CWrapper::unlock();
    
    m_accel_conf = range;
}

int  Mpu6050::accel_get_full_scale_range(){
    I2CWrapper::lock();
    uint8_t value = I2CWrapper::I2CReadReg8(m_fd, MPU6050_ACCEL_CONFIG);
    I2CWrapper::unlock();

    return (value & 0x18) >> 3;
}

// Set device to to sleep (true) or wake up (false)
void Mpu6050::set_sleep(bool sleep_mode){
    I2CWrapper::lock();
    uint8_t value = I2CWrapper::I2CReadReg8(m_fd, MPU6050_PWR_MGMT_1);

    value = (sleep_mode ? SET_BIT(value, MPU6050_D6) : (value&0xBF));
    
    I2CWrapper::I2CWriteReg8(m_fd, MPU6050_PWR_MGMT_1, value);
    m_sleep_mode = sleep_mode;
    I2CWrapper::unlock();
    delay(100);
}

//get sleep mode from device
bool Mpu6050::get_sleep(){
    I2CWrapper::lock();
    uint8_t value = I2CWrapper::I2CReadReg8(m_fd, MPU6050_PWR_MGMT_1);
    I2CWrapper::unlock();
    return IF_BIT(value, MPU6050_D6);
}

/*
* Run self check procedure
*/
void Mpu6050::self_check(){
    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, MPU6050_ACCEL_CONFIG, 0xE0);
    I2CWrapper::I2CWriteReg8(m_fd, MPU6050_GYRO_CONFIG, 0xE0);
    I2CWrapper::unlock();

    delay(500);

    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, MPU6050_ACCEL_CONFIG, 0x00);
    I2CWrapper::I2CWriteReg8(m_fd, MPU6050_GYRO_CONFIG, 0x00);
    I2CWrapper::unlock();

    delay(100);
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
    int error = 0;
    accel_t_gyro_union* accel_t_gyro = (accel_t_gyro_union *) accel_t_gyro_ptr;

    I2CWrapper::lock();
    //int error = I2CWrapper::I2CReadData(m_fd, MPU6050_ACCEL_XOUT_H, (uint8_t *) accel_t_gyro, sizeof(*accel_t_gyro));
    accel_t_gyro->reg.x_accel_h = I2CWrapper::I2CReadReg8(m_fd, MPU6050_ACCEL_XOUT_H);
    accel_t_gyro->reg.x_accel_l = I2CWrapper::I2CReadReg8(m_fd, MPU6050_ACCEL_XOUT_L);
    accel_t_gyro->reg.y_accel_h = I2CWrapper::I2CReadReg8(m_fd, MPU6050_ACCEL_YOUT_H);
    accel_t_gyro->reg.y_accel_l = I2CWrapper::I2CReadReg8(m_fd, MPU6050_ACCEL_YOUT_L);
    accel_t_gyro->reg.z_accel_h = I2CWrapper::I2CReadReg8(m_fd, MPU6050_ACCEL_ZOUT_H);
    accel_t_gyro->reg.z_accel_l = I2CWrapper::I2CReadReg8(m_fd, MPU6050_ACCEL_ZOUT_L);

    accel_t_gyro->reg.x_gyro_h = I2CWrapper::I2CReadReg8(m_fd, MPU6050_GYRO_XOUT_H);
    accel_t_gyro->reg.x_gyro_l = I2CWrapper::I2CReadReg8(m_fd, MPU6050_GYRO_XOUT_L);
    accel_t_gyro->reg.y_gyro_h = I2CWrapper::I2CReadReg8(m_fd, MPU6050_GYRO_YOUT_H);
    accel_t_gyro->reg.y_gyro_l = I2CWrapper::I2CReadReg8(m_fd, MPU6050_GYRO_YOUT_L);
    accel_t_gyro->reg.z_gyro_h = I2CWrapper::I2CReadReg8(m_fd, MPU6050_GYRO_ZOUT_H);
    accel_t_gyro->reg.z_gyro_l = I2CWrapper::I2CReadReg8(m_fd, MPU6050_GYRO_ZOUT_L);

    accel_t_gyro->reg.t_h = I2CWrapper::I2CReadReg8(m_fd, MPU6050_TEMP_OUT_H);
    accel_t_gyro->reg.t_l = I2CWrapper::I2CReadReg8(m_fd, MPU6050_TEMP_OUT_L);

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
  char buff[2048]; 

  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started");

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

  std::sprintf(buff, " Base Accel [X:%.3f Y:%.3f Z:%.3f] Gyro [X:%.3f Y:%.3f Z:%.3f]",
  base_x_accel, base_y_accel, base_z_accel,
  base_x_gyro, base_y_gyro, base_z_gyro);

  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(buff));

  std::sprintf(buff, " Base Accel [X:%.3f Y:%.3f Z:%.3f] Gyro [X:%.3f Y:%.3f Z:%.3f]",
  base_x_accel/get_accel_sens(), base_y_accel/get_accel_sens(), base_z_accel/get_accel_sens(),
  base_x_gyro/get_gyro_sens(), base_y_gyro/get_gyro_sens(), base_z_gyro/get_gyro_sens());

  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + std::string(buff));

  logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Finished");
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

    // The temperature sensor is -40 to +85 degrees Celsius.
    // It is a signed integer.
    // According to the datasheet:
    //   340 per degrees Celsius, -512 at 35 degrees.
    // At 0 degrees: -512 - (340 * 35) = -12412
    temperature = ( (double) accel_t_gyro.value.temperature + 12412.0) / 340.0;

    // Convert gyro values to degrees/sec
    float FS_SEL = get_gyro_sens();
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

    std::sprintf(buff, "Time:%ld Angle [X:%.3f Y:%.3f Z:%.3f] Gyro [X:%.3f Y:%.3f Z:%.3f] Temp:%.2f", 
    val.last_read_time,
    val.last_x_angle/get_accel_sens(),
    val.last_y_angle/get_accel_sens(),
    val.last_z_angle/get_accel_sens(),
    val.last_gyro_x_angle/get_gyro_sens(),
    val.last_gyro_y_angle/get_gyro_sens(), 
    val.last_gyro_z_angle/get_gyro_sens(), 
    val.last_temperature);

    return std::string(buff);
}

/*
 *
 */
void Mpu6050::stop(){
    piutils::Threaded::stop();
}

/*
 *
 */
bool Mpu6050::start(void)
{
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Started...");
    return piutils::Threaded::start<Mpu6050>(this);	
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
void Mpu6050::worker(Mpu6050* owner){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker started.");
    while(!owner->is_stop_signal()){
        /*
         *
         */
        owner->update_values();
        //logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + owner->print_current());

        //delay(owner->get_loop_delay());
        std::this_thread::sleep_for(std::chrono::milliseconds(owner->get_loop_delay()));
    }

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Worker finished.");
}


} /* namespace mpu6050 */
} /* namespace pirobot */
