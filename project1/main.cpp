#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#include "version.h"

#include "defines.h"
#include "StateMachine.h"
#include "MyStateFactory.h"
#include "PiRobotPrj1.h"

using namespace std;
smachine::StateMachine* stm;
pid_t childPid;

static void sigHandlerChild(int sign){
  if(sign == SIGINT) {
    stm->finish();
  }
  else if (sign == SIGUSR1){
    stm->run();
  }
}

static void sigHandlerParent(int sign){
  if (sign == SIGINT || sign == SIGUSR1) {
    kill(childPid, sign);
  }
}


int main (int argc, char* argv[])
{
  cout <<  "Project1 started" << endl;

  switch(childPid = fork()){
    case -1:
      exit(EXIT_FAILURE);

    case 0: //child
      {
        if (signal(SIGINT, sigHandlerChild) == SIG_ERR){
          _exit(EXIT_FAILURE);
        }

        std::shared_ptr<project1::MyStateFactory> factory(new project1::MyStateFactory());
        std::shared_ptr<pirobot::PiRobot> pirobot(new project1::PiRobotPrj1());
    
        stm = new smachine::StateMachine(factory, pirobot);
        stm->wait();
        delete stm;

        _exit(EXIT_SUCCESS);
      }
      break;

    default: //parent
      if (signal(SIGINT, sigHandlerParent) == SIG_ERR){
        _exit(EXIT_FAILURE);
      }

      for(;;){
        pid_t chdPid = wait(NULL);
        if(chdPid == -1){
          if (errno == ECHILD) {
            cout <<  "No more childs" << endl;
            break;
          }
        }
        else
          cout <<  "Child finished " <<  chdPid << endl;
      }
      cout <<  "Project1 finished" << endl;
  }

  exit(EXIT_SUCCESS);
}

