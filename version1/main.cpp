#include <iostream>
#include <unistd.h>
#include <stdlib.h>

//#include <wiringPi.h>
#include "version.h"
#include "RealWorld.h"
#include "Adafruit_PWMServoDriver.h"

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
#define	LED	0

using namespace std;

int main (int argc, char* argv[])
{
  cout <<  "Raspberry Pi blink Parameters:" << argc << endl;

  float freq = 60.0;
  uint16_t high = 4095;
  uint16_t low = 4095;
  int delay  = 2;  

  if(argc == 5){
     freq = stof(argv[1]);
     high = (uint16_t)stoi(argv[2]);
     low  = (uint16_t)stoi(argv[3]);
     delay = stoi(argv[4]); 
  }

  cout <<  "Parameters Freq:" << freq << " High: " << high << " Low: " << low << " Delay: " << delay  << endl;

  pirobot::gpio::Adafruit_PWMServoDriver* D9685 = new pirobot::gpio::Adafruit_PWMServoDriver();
  D9685->begin();

  D9685->setPWMFreq(freq);

  cout <<  "Set 1 Low" << endl;
  D9685->setPWM(3, 0, low);
  D9685->setPWM(0, 0, low);
  sleep(delay);

  //cout <<  "Set 2 High" << endl;
  //D9685->setPWM(3, high, 0);
  //D9685->setPWM(0, high, 0);
  //sleep(delay);

  cout <<  "Set 3 Off" << endl;
  D9685->setPWM(0, 0, 0);
  D9685->setPWM(3, 0, 0);

  //sleep(5);

  //D9685->reset(); 

/*
  realworld::RealWorld* robot = new realworld::RealWorld();

  robot->configure();
  robot->start();

  for(int i = 0; i < 10; i++){
	  cout <<  "Wait loop " << i << endl;
	  sleep(1);
  }

  cout <<  "Stop robot " << endl;
  robot->stop();

  cout <<  "Release robot " << endl;
  delete robot;
*/

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
