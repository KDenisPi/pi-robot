#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdexcept>
#include <exception>

#include "wiringPi.h"
#include "version.h"
//#include "RealWorld.h"
//#include "Adafruit_PWMServoDriver.h"
#include "Mpu6050.h"


// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
#define	LED	0
#define SERVO   7

#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

using namespace std;

int main (int argc, char* argv[])
{
  cout <<  "Raspberry Pi blink Parameters:" << argc << endl;

  pirobot::mpu6050::Mpu6050* mpu = new  pirobot::mpu6050::Mpu6050();
  int res = wiringPiSetup();
  cout <<  "Started wiringPi" << endl;

  mpu->start();
  cout <<  "Started MPU6050" << endl;
  cout << mpu->print_current() << endl;

  for(int i =0; i < 20; i++){
   cout << mpu->print_current() << endl;
   sleep(1);
  }

  cout <<  "Stop MPU6050" << endl;
  mpu->stop();

  cout <<  "Delete MPU6050" << endl;
  delete mpu;

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

/*
  realworld::RealWorld* robot = new realworld::RealWorld();

  robot->configure();
  robot->printConfig();
  robot->start();

  sleep(4);
  cout <<  "Wait ...." << endl;

  cout <<  "Stop robot " << endl;
  robot->stop();

  cout <<  "Release robot " << endl;
  delete robot;

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
