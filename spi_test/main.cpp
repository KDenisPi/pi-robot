#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <fstream>


#include "logger.h"
#include "StateMachine.h"
#include "MyStateFactory.h"

using namespace std;
smachine::StateMachine* stm;
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

int main (int argc, char* argv[])
{
  cout <<  "spi_test started" << endl;

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

        std::shared_ptr<spi_test::MyStateFactory> factory(new spi_test::MyStateFactory());
        std::shared_ptr<pirobot::PiRobot> pirobot(new pirobot::PiRobot());
        std::shared_ptr<mqqt::MqqtClient<mqqt::MqqtClientItf>> mqqt();
        stm = new smachine::StateMachine(factory, pirobot, nullptr);

        cout <<  "State machine started. Wait" << endl;   
        cout <<  "Logger links: " << logger::plog.use_count() << endl;
        stm->wait();

        cout  <<  "State machine finished" << endl;         

        if(logger::plog){
          cout <<  "Check logger state" << endl;
          cout <<  "Logger links: " << logger::plog.use_count() << endl;
        }
        else
          cout <<  "Logger not present" << endl;

        sleep(2);
        cout <<  "Delete State Machine" << endl;
        delete stm;

        cout <<  "State Machine deleted" << endl;
        if(logger::plog){
          cout <<  "Logger links 2: " << logger::plog.use_count() << endl;
        }

        logger::release();       
 
        _exit(EXIT_SUCCESS);
      }
      break;

    default: //parent
      cout <<  "State machine child created " <<  stmPid << endl;

      ADD_SIGNAL(SIGALRM)

      if( signal(SIGUSR1, sigHandlerParent) == SIG_ERR){
        cout <<  "Parent handler error " <<  stmPid << endl;
      }

      if (signal(SIGINT, sigHandlerParent) == SIG_ERR ){
        cout <<  "Parent handler error " <<  stmPid << endl;
        kill(stmPid, SIGINT);
      }

      sleep(5);
      kill(stmPid, SIGUSR1);

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

