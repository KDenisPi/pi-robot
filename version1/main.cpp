#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdexcept>
#include <exception>

#include "wiringPi.h"
#include "version.h"
#include "RealWorld.h"
//#include "Adafruit_PWMServoDriver.h"
//#include "Mpu6050.h"
#include "DRV8825StepperMotor.h"
#include "DRV8834StepperMotor.h"
#include "mservo.h"
#include "DCMotor.h"
#include "ULN2003StepperMotor.h"

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
#define	LED	0
#define SERVO   7

#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

using namespace std;

int main (int argc, char* argv[])
{
  cout <<  "Raspberry Pi blink Parameters:" << argc << endl;

/*
  pirobot::mpu6050::Mpu6050* mpu = new  pirobot::mpu6050::Mpu6050(0x68, 100);
  int res = wiringPiSetup();
  cout <<  "Started wiringPi" << endl;

  mpu->start();
  cout <<  "Started MPU6050" << endl;
  cout << mpu->print_current() << endl;

  for(int i =0; i < 20; i++){
   cout << mpu->print_current() << endl;
   sleep(1);
  }


 sleep(10); 

  cout <<  "Stop MPU6050" << endl;
  mpu->stop();

  cout <<  "Delete MPU6050" << endl;
  delete mpu;
*/

/*
  float freq = 60.0;
  uint16_t high = 4095;
  uint16_t low = 4095;
  int delay  = 4;
  uint16_t steps = 10;
  int rightOnly = 0;

  if(argc == 3){
     steps = stoi(argv[1]); 
     rightOnly = stoi(argv[2]); 
  }
*/
/*
  if(argc == 5){
     freq = stof(argv[1]);
     high = (uint16_t)stoi(argv[2]);
     low  = (uint16_t)stoi(argv[3]);
     delay = stoi(argv[4]); 
  }
*/
  //cout <<  "Parameters Freq:" << freq << " High: " << high << " Low: " << low << " Delay: " << delay  << endl;
/*
  cout <<  " Steps: " << steps << endl;

  pirobot::gpio::Adafruit_PWMServoDriver* D9685 = new pirobot::gpio::Adafruit_PWMServoDriver();
  D9685->begin();
  D9685->setPWMFreq(freq);

  	
  cout <<  "Set turn" << endl;
  struct pirobot::gpio::LED_DATA ldata;
  ldata = {0,0};
  D9685->getPin(SERVO, ldata);
  cout <<  "Start Get On: " << ldata.on << " Off: " << ldata.off << endl;
  sleep(5);

  for(uint16_t pulselen = SERVOMIN; pulselen < SERVOMIN + steps; pulselen++){
       D9685->setPWM(SERVO, 0, SERVOMIN + pulselen);
       ldata = {0,0};
       D9685->getPin(SERVO, ldata);
       cout <<  "Get On: " << ldata.on << " Off: " << ldata.off << endl;
  }

    //D9685->setPWM(SERVO, 0, 0);
  sleep(2);

  D9685->getPin(SERVO, ldata);
  cout <<  "Start back On: " << ldata.on << " Off: " << ldata.off << endl;
  if(rightOnly == 0){
    for(uint16_t pulselen = ldata.off; pulselen > SERVOMIN; pulselen--){
     D9685->setPWM(SERVO, 0, pulselen);
    }
    //D9685->setPWM(SERVO, 0, 0);
  }
  //D9685->reset(); 
   *
   */

  realworld::RealWorld* robot = new realworld::RealWorld();

  robot->configure();
  robot->printConfig();
  robot->start();

  sleep(3);
  cout <<  "Get motors ...." << endl;
  //auto stepper1 = dynamic_cast<pirobot::item::DRV8825_StepperMotor*>(robot->get_item("STEP_1").get());
  //stepper1->step();
  //auto stm1 = dynamic_cast<pirobot::item::ServoMotor*>(robot->get_item("SMT_1").get());
  //auto stm2 = dynamic_cast<pirobot::item::ServoMotor*>(robot->get_item("SMT_2").get());
  auto step_1 = dynamic_cast<pirobot::item::DRV8834_StepperMotor*>(robot->get_item("STEP_1").get());
  //auto step_1 = dynamic_cast<pirobot::item::ULN2003StepperMotor*>(robot->get_item("STEP_1").get());
  //auto dcm1 = dynamic_cast<pirobot::item::dcmotor::DCMotor*>(robot->get_item("DCM_1").get()); 

  //step_1->set_step_size(pirobot::item::DRV8834_MICROSTEP::STEP34_1_2);
  int i;
  for(i = 0; i < 10; i++){
	step_1->step(1);
	sleep(5);
  }
/*
  step_1->set_direction(pirobot::item::MOTOR_DIR::DIR_COUTERCLOCKWISE);

  for(i = 0; i < 5; i++){
	step_1->step(100);
	sleep(1);
  }
*/

/*
  cout <<  "Servo 0 ...." << endl;
  stm1->setAngle(0.0f);
  stm2->setAngle(0.0f);
  sleep(3);

  cout <<  "Servo -90 ...." << endl;
  stm1->setAngle(-90.0f);
  stm2->setAngle(90.0f);
  sleep(3);

  cout <<  "Servo 0 ...." << endl;
  stm1->setAngle(0.0f);
  stm2->setAngle(0.0f);
  sleep(3);

  cout <<  "Servo +90 ...." << endl;
  stm1->setAngle(90.0f);
  stm2->setAngle(-90.0f);
  sleep(3);

  cout <<  "Servo 0 ...." << endl;
  stm1->setAngle(0.0f);
  stm2->setAngle(0.0f);
  sleep(5);
*/

/*
  cout <<  "DC Motor 10% ...." << endl;
  dcm1->set_power_level(10.0f);
  sleep(5);

  cout <<  "DC Motor 50% ...." << endl;
  dcm1->set_power_level(50.0f);
  sleep(5);

  cout <<  "DC Motor 100% ...." << endl;
  dcm1->set_power_level(100.0f);
  sleep(5);

  cout <<  "DC Motor stop ...." << endl;
  dcm1->stop();
  sleep(5);

  cout <<  "DC Motor change direction and 50% ...." << endl;
  dcm1->set_direction( pirobot::item::MOTOR_DIR::DIR_COUTERCLOCKWISE);
  dcm1->set_power_level(50.0f);
  sleep(5);

  cout <<  "DC Motor 100% ...." << endl;
  dcm1->set_power_level(100.0f);
  sleep(5);

  cout <<  "DC Motor 10% ...." << endl;
  dcm1->set_power_level(10.0f);
  sleep(5);

  cout <<  "DC Motor stop ...." << endl;
  dcm1->stop();
*/
  sleep(5);


  cout <<  "Stop robot " << endl;
  robot->stop();

  cout <<  "Release robot " << endl;
  delete robot;

  /*

  wiringPiSetup () ;
  pinMode (LED, OUTPUT) ;

  cout << "WiringPi initialized" << endl;


  for (int i=0; i<20; i++)
  {
    digitalWrite (LED, HIGH) ;	// On
    delay (500) ;	// mS

    //cout << "LED On" << endl;

    digitalWrite (LED, LOW) ;	// Off

   // cout << "LED Off" << endl; 

    delay (500) ;
  }

  digitalWrite(LED, LOW);
*/
  exit(0);
}
