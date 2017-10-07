#include <iostream>
#include <unistd.h>
#include <stdlib.h>

/*
* Timer test
*/
//#include "Timers.h"

#include "StateMachine.h"
#include "MyStateFactory.h"
#include "PiRobotPrj1.h"

using namespace std;

int main (int argc, char* argv[])
{
  cout <<  "spi_test started" << endl;

  std::shared_ptr<spi_test::MyStateFactory> factory(new spi_test::MyStateFactory());
  std::shared_ptr<pirobot::PiRobot> pirobot(new spi_test::PiRobotPrj1(true));

  smachine::StateMachine* stm = new smachine::StateMachine(factory, pirobot);

  sleep(4);

  cout <<  "Wait for state machine" << endl;
  stm->wait();

  cout <<  "Sleep 3 sec" << endl;
  sleep(3);

  cout <<  "Delete State Machine" << endl;
  delete stm;
  cout <<  "spi_test finished" << endl;
  exit(0);
}

