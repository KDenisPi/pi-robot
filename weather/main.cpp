#include <fstream>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#include "MosquittoClient.h"
#include "MqqtClient.h"
#include "CircularBuffer.h"

#include "version.h"
#include "defines.h"

#include "StateMachine.h"
#include "context.h"
#include "WeatherStFactory.h"
#include "WebSettings.h"


using namespace std;

smachine::StateMachine* stm;
void mytime(const std::string& message);

pid_t stmPid;

void mytime(const std::string& message){
    char mtime[30];
    std::chrono::time_point<std::chrono::system_clock> tp;
    tp = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(tp);
    std::strftime(mtime, sizeof(mtime), "%T", std::localtime(&time_now));
    std::cout << mtime << " --- " << message << std::endl;
}

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
    if(stmPid){
      cout <<  "Parent: Send signal to child  " << sign  << endl;
      kill(stmPid, sign);
    }
  }
}

const char* err_message = "Error. No configuration file.\nUsage weather --conf coniguration_file [--mqqt-conf mqqt_configuration_file ]";

std::string validate_file_parameter(const int idx, const int argc, char* argv[]){
  std::string filename;
  if(idx == argc){
    cout <<  err_message << endl;
    _exit(EXIT_FAILURE);
  }

  filename = argv[idx];
  std::ifstream file_stream(filename);
  if(!file_stream){
    cout <<  "Configuration file " << filename << " does not exist or not available." << endl;
    _exit(EXIT_FAILURE);
  }
  file_stream.close();

  return filename;
}

/*
* program --conf path_to_configuration [--mqqt mqqt_server_ip_addres]
*    --mqqt - optional if MQQT client should be used
*/
int main (int argc, char* argv[])
{
  bool mqtt = false;
  std::string robot_conf, mqqt_conf;
  stmPid  = 0;
  sigset_t new_set;
  cout <<  "Weather started" << endl;

  for(int i = 0; i < argc; i++){
    std::string arg = argv[i];
    cout <<  "Arg: " << i << " [" << arg << "]" << endl;

    if(strcmp(argv[i], "--conf") == 0){
      robot_conf = validate_file_parameter(++i, argc, argv);
    }
    else if(strcmp(argv[i], "--mqqt-conf") == 0){
      mqqt_conf = validate_file_parameter(++i, argc, argv);
      mqtt = true;
    }
  }

  if(robot_conf.empty()){
    cout <<  err_message << endl;
    _exit(EXIT_FAILURE);
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

        logger::log(logger::LLOG::DEBUG, "main", std::string(__func__) + " Create child");

        /*
        * Create PI Robot instance
        */
        cout <<  "Create hardware support" << endl;
        std::shared_ptr<pirobot::PiRobot> pirobot(new pirobot::PiRobot());
        pirobot->set_configuration(robot_conf);

        //Create State factory for State Machine
        cout <<  "Create State Factory support" << endl;
        std::shared_ptr<weather::WeatherStFactory> factory(new weather::WeatherStFactory());

        std::shared_ptr<mqqt::MqqtItf> clMqqt;
        if(mqtt){
              cout <<  "MQQT detected" << endl;
              try{
                int res = mosqpp::lib_init();
                cout <<  "MQQT library intialized Res: " << std::to_string(res) << endl;

                // Load MQQT server configuration
                mqqt::MqqtServerInfo info = mqqt::MqqtServerInfo::load(mqqt_conf);
                clMqqt = std::shared_ptr<mqqt::MqqtItf>(new mqqt::MqqtClient<mqqt::MosquittoClient>(info));
                cout <<  "MQQT configuration loaded" << endl;
              }
              catch(std::runtime_error& rterr){
                cout <<  "Could not load MQQT configuration configuration " << robot_conf << "\nError: " << rterr.what() << endl;
                _exit(EXIT_FAILURE);
              }
        }

        /*
        * Web interface for settings and status
        */
        std::shared_ptr<weather::WebSettings> web(new weather::WebSettings(8080));
        web->start();

        /*
        * Create State machine
        */
        stm = new smachine::StateMachine(factory, pirobot, clMqqt);
        cout <<  "Created state machine. Waiting for finishing" << endl;
        stm->wait();

        cout <<  "State machine finished" << endl;

        sleep(2);
        delete stm;

        if(mqtt){
          mosqpp::lib_cleanup();
        }

        logger::log(logger::LLOG::DEBUG, "main", std::string(__func__) + " Child finished");
        logger::release();

        _exit(EXIT_SUCCESS);
      }
      break;

    default: //parent
      cout <<  "State machine created " <<  stmPid << endl;

      if( signal(SIGUSR1, sigHandlerParent) == SIG_ERR){
        cout <<  "Parent handler error " <<  stmPid << endl;
      }

      if (signal(SIGINT, sigHandlerParent) == SIG_ERR ){
        cout <<  "Parent handler error " <<  stmPid << endl;
        kill(stmPid, SIGINT);
      }

      sleep(2);

      //send command to start
      kill(stmPid, SIGUSR1);

      //wait  untill child will not be finished
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

