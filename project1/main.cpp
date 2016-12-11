#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <memory>
#include <pthread.h>

#include "version.h"

/*
* Timer test
*/
//#include "Timers.h"

#include "defines.h"
#include "StateMachine.h"
#include "MyStateFactory.h"
#include "PiRobot.h"

using namespace std;

int main (int argc, char* argv[])
{
  cout <<  "Project1 started" << endl;

  std::shared_ptr<project1::MyStateFactory> factory(new project1::MyStateFactory());
  std::shared_ptr<pirobot::PiRobot> pirobot(new pirobot::PiRobot());

  smachine::StateMachine* stm = new smachine::StateMachine(factory, pirobot);

  sleep(4);

  cout <<  "Wait for state machine" << endl;
  void* ret;
  int res = pthread_join(stm->get_thread(), &ret);
  if(res != 0)
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not join to thread Res:" + std::to_string(res));

  cout <<  "Sleep 3 sec" << endl;
  sleep(3);

  cout <<  "Delete State Machine" << endl;
  delete stm;
  cout <<  "Project1 finished" << endl;
  exit(0);
}

