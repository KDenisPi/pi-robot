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
#include "StateMashine.h"
#include "MyStateFactory.h"
#include "PiRobot.h"

using namespace std;

int main (int argc, char* argv[])
{
  cout <<  "Project1 started" << endl;

  std::shared_ptr<project1::MyStateFactory> factory(new project1::MyStateFactory());
  std::shared_ptr<pirobot::PiRobot> pirobot(new pirobot::PiRobot());

  std::shared_ptr<smashine::StateMashine> stm(new smashine::StateMashine(factory, pirobot));

  sleep(2);

  cout <<  "Wait for state machine" << endl;
  void* ret;
  int res = pthread_join(stm->get_thread(), &ret);
  if(res != 0)
		logger::log(logger::LLOG::ERROR, TAG, std::string(__func__) + " Could not join to thread Res:" + std::to_string(res));


  cout <<  "Project1 finished" << endl;
  exit(0);
}

