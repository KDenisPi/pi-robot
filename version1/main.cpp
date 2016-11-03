#include <iostream>
#include <wiringPi.h>
#include "version.h"

using namespace std;

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.

#define	LED	0

int main (void)
{
  cout <<  "Raspberry Pi blink" << endl;

  wiringPiSetup () ;
  pinMode (LED, OUTPUT) ;

  cout << "WiringPi initilized" << endl;


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

  return(0);
}
