#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#include "version.h"

#include "defines.h"
#include "StateMachine.h"
#include "MyStateFactory.h"
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

  cout <<  "MQQT: Detected signal " << sign  << endl;
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
 cout <<  "Parent: Detected signal " << sign  << endl;

  if (sign == SIGINT || sign == SIGUSR1) {
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
  stmPid = mqqtPid = 0;
  sigset_t new_set;
  cout <<  "Project1 started" << endl;

  if(argc >= 2 && strcmp(argv[1], "--mqtt") == 0){
    mqtt = true;
  }

  switch(stmPid = fork()){
    case -1:
      cout <<  "Failed first fork" << endl;
      exit(EXIT_FAILURE);

    case 0: //child
      {
        ADD_SIGNAL(SIGALRM)

        if (signal(SIGINT, sigHandlerStateMachine) == SIG_ERR ){
             cout <<  "Failed set first fork handler" << endl;
             _exit(EXIT_FAILURE);
        }

        if( signal(SIGUSR1, sigHandlerStateMachine) == SIG_ERR){
             cout <<  "Failed set first fork handler" << endl;
             _exit(EXIT_FAILURE);
        }

        std::shared_ptr<project1::MyStateFactory> factory(new project1::MyStateFactory());
        std::shared_ptr<pirobot::PiRobot> pirobot(new pirobot::PiRobot());

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
              if (signal(SIGINT, sigHandlerMQQT) == SIG_ERR ||
                    signal(SIGUSR1, sigHandlerMQQT) == SIG_ERR){
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

      if( signal(SIGUSR1, sigHandlerParent) == SIG_ERR){
        cout <<  "Parent handler error " <<  stmPid << endl;
      }

      if (signal(SIGINT, sigHandlerParent) == SIG_ERR ){
        cout <<  "Parent handler error " <<  stmPid << endl;
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

