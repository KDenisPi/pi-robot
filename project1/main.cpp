#include <fstream>
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

const char* err_message = "Error. No configuration file.\nUsage project1 --conf coniguration_file [--mqqt mqqt_server_ip_addres]";

/*
* program --conf path_to_configuration [--mqqt mqqt_server_ip_addres]
*    --mqqt - optional if MQQT client should be used
*/
int main (int argc, char* argv[])
{
  bool mqtt = false;
  std::string conf_file, mqqt_ip;
  stmPid = mqqtPid = 0;
  sigset_t new_set;
  cout <<  "Project1 started" << endl;

  if(argc < 3 || (strcmp(argv[1], "--conf") != 0)){
    cout <<  err_message << endl;
    _exit(EXIT_FAILURE);
  }

  conf_file = argv[3];
  std::ifstream file_stream(argv[3]);
  if(!file_stream){
    cout <<  "Configuration file does not exist or not available." << endl;
    _exit(EXIT_FAILURE);
  }
  file_stream.close();

  if(argc >= 5 && strcmp(argv[3], "--mqtt") == 0){
    mqtt = true;
    //IP address of MQQT server
    mqqt_ip = argv[4];
  }

  switch(stmPid = fork()){
    case -1:
      cout <<  "Could not create state machine application" << endl;
      exit(EXIT_FAILURE);

    case 0: //child
      {
        /*
        * Add handler for SIGALARM signal (used for State Machine Timers)
        */
        ADD_SIGNAL(SIGALRM)

        /*
        * Add handler  for SIGINT signal
        */
        if (signal(SIGINT, sigHandlerStateMachine) == SIG_ERR ){
             cout <<  "Failed to set SIGINT handler for state machine" << endl;
             _exit(EXIT_FAILURE);
        }

        /*
        * Add handler  for SIGUSR1 signal. This signal will be used for State Machine start.
        */
        if( signal(SIGUSR1, sigHandlerStateMachine) == SIG_ERR){
             cout <<  "Failed to set SIGUSR1 handler for state machine" << endl;
             _exit(EXIT_FAILURE);
        }

        /*
        * Create PI Robot instance
        */
        std::shared_ptr<pirobot::PiRobot> pirobot(new pirobot::PiRobot());

        try{
          // Load hardware configuration
          pirobot->configure(conf_file);
        }
        catch(std::runtime_error& rterr){
          cout <<  "Could not load Pi Robot hardware configuration.\nError: " << rterr.what() << endl;
          _exit(EXIT_FAILURE);
        }

        //Create State factory for State Machine
        std::shared_ptr<project1::MyStateFactory> factory(new project1::MyStateFactory());

        /*
        * Create State machine
        */
        stm = new smachine::StateMachine(factory, pirobot);
        cout <<  "Created state machine. Waiting for finishing" << endl;
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

              mqqt::MqqtServerInfo info(mqqt::MqqtServerInfo(mqqt_ip, "pi-robot"));
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

