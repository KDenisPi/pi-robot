#include <iostream>
#include <unistd.h>
#include <stdlib.h>

//#include <wiringPi.h>
#include "version.h"
#include "PiRobot.h"

using namespace std;

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.

#define	LED	0

int main (void)
{
  cout <<  "Raspberry Pi blink" << endl;
  pirobot::PiRobot* robot = new pirobot::PiRobot();

  robot->configure();
  robot->start();

  sleep(2);

  cout <<  "Stop robot " << endl;
  robot->stop();

  cout <<  "Release robot " << endl;
  delete robot;

  for(int i = 0; i < 10; i++){
	  cout <<  "Wait loop " << i << endl;
	  sleep(1);
  }

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
