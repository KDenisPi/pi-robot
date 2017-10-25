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
#include "MosquittoClient.h"
#include "MqqtClient.h"

using namespace std;

smachine::StateMachine* stm;
mqqt::MqqtClient<mqqt::MosquittoClient>* clMqqt;

pid_t stmPid, mqqtPid;

/*
* Singnal handler for State Machine
*/
static void sigHandlerStateMachine(int sign){

  cout <<  "State machine: Detected signal " << sign  << endl;
  if(sign == SIGINT) {
    stm->finish();
  }
  else if (sign == SIGUSR1){
    stm->run();
  }
}

/*
* Singnal handler for MQQT client
*/
static void sigHandlerMQQT(int sign){

  cout <<  "State machine: Detected signal " << sign  << endl;
  if(sign == SIGINT) {
    clMqqt->stop();
  }
  else if (sign == SIGUSR1){
    clMqqt->start();
  }
}


/*
* Singnal handler for parent
*/
static void sigHandlerParent(int sign){

  if (sign == SIGINT || sign == SIGUSR1) {

    cout <<  "Parent: Detected signal " << sign  << endl;
    if(stmPid)
      kill(stmPid, sign);
    if(mqqtPid)  
      kill(mqqtPid, sign);
  }
}

/*
*
*/
int main (int argc, char* argv[])
{
  bool mqtt = false;
  sigset_t new_set;
  stmPid = mqqtPid = 0;
  
  cout <<  "Project1 started" << endl;

  sigemptyset (&new_set);
  sigaddset (&new_set, SIGUSR1);
  if( sigprocmask(SIG_BLOCK, &new_set, NULL) < 0){
      cout <<  " Could not set signal mask." << endl;
      exit(EXIT_FAILURE);
  }

  if(argc >= 2 && strcmp(argv[1], "--mqtt") == 0){
    mqtt = true;
  }

  switch(stmPid = fork()){
    case -1:
      cout <<  "Failed first fork" << endl;
      exit(EXIT_FAILURE);

    case 0: //child
      {
        if (signal(SIGINT, sigHandlerStateMachine) == SIG_ERR){
         cout <<  "Failed set first fork handler" << endl;
          _exit(EXIT_FAILURE);
        }

        std::shared_ptr<project1::MyStateFactory> factory(new project1::MyStateFactory());
        std::shared_ptr<pirobot::PiRobot> pirobot(new project1::PiRobotPrj1());

        stm = new smachine::StateMachine(factory, pirobot);
        cout <<  "Created state machine, wait" << endl;
        stm->wait();
        cout <<  "State machine finished" << endl;

        sleep(2);

        delete stm;

        _exit(EXIT_SUCCESS);
      }
      break;

    default: //parent
      cout <<  "State machine child created " <<  stmPid << endl;

      if(mqtt){
        switch(mqqtPid = fork()){
          case -1:
            //exit(EXIT_FAILURE);
            break;
          case 0:
            {
              if (signal(SIGINT, sigHandlerMQQT) == SIG_ERR){
                _exit(EXIT_FAILURE);
              }

              mqqt::MqqtServerInfo info(mqqt::MqqtServerInfo("10.0.0.9", "pi-robot"));
              clMqqt = new mqqt::MqqtClient<mqqt::MosquittoClient>(info);
              clMqqt->wait();

              sleep(2);

              delete clMqqt;
              _exit(EXIT_SUCCESS);
            }
            break;

          default:
            cout <<  "MQTT client child created " <<  mqqtPid << endl;
        }
      }

      if (signal(SIGINT, sigHandlerParent) == SIG_ERR){
        kill(stmPid, SIGINT);
        
        if(mqtt)
          kill(mqqtPid, SIGINT);
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

