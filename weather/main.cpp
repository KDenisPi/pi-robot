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
#include "WebSettingsWeather.h"


using namespace std;

/*
* State Machine instance
*/
std::shared_ptr<smachine::StateMachine> stm;

/*
* State Machine process ID
*/
pid_t stmPid;

/*
* Daemon flag
*/
bool daemon_mode = true;

/*
* Default debug level
*/
logger::LLOG dlevel = logger::LLOG::INFO;

/*
*
*/
std::string firstState = "StInitialization";

/*
* Test purpose function
*/
void mytime(const std::string& message);
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

  //cout <<  "State machine: Detected signal " << sign  << endl;
  if(sign == SIGUSR2  || sign == SIGTERM) {
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
      kill(stmPid, (sign == SIGINT ? SIGUSR2 : sign));
    }
  }
}

const char* err_message = "Error. No configuration file.";
const char* help_message = "weather --conf coniguration_file [--mqqt-conf mqqt_configuration_file] [--nodaemon] [--dlevel INFO|DEBUG|NECECCARY|ERROR]";

/*
*
*/
std::string validate_file_parameter(const int idx, const int argc, char* argv[]){
  std::string filename;
  if(idx == argc){
    cout <<  err_message << endl;
    cout <<  help_message << endl;
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
* program --conf path_to_configuration [--mqqt-conf mqqt_configuration_file]
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
    else if(strcmp(argv[i], "--nodaemon") == 0){
      daemon_mode = false;
    }
    else if(strcmp(argv[i], "--dlevel") == 0){
      if(++i < argc){
        if(strcmp(argv[i], "INFO") == 0) dlevel = logger::LLOG::INFO;
        else if(strcmp(argv[i], "DEBUG") == 0) dlevel = logger::LLOG::DEBUG;
        else if(strcmp(argv[i], "ERROR") == 0) dlevel = logger::LLOG::ERROR;
        else if(strcmp(argv[i], "NECECCARY") == 0) dlevel = logger::LLOG::NECECCARY;
      }
    }
    else if(strcmp(argv[i], "--fstate") == 0){
      if(++i < argc){
        firstState = argv[i];
      }
    }
  }

  if(robot_conf.empty()){
    cout <<  err_message << endl;
    cout <<  help_message << endl;
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
        * Add handler  for SIGUSR2 signal - Used for State Machine finishing
        */
        if (signal(SIGUSR2, sigHandlerStateMachine) == SIG_ERR ){
          //cout <<  "Failed to set SIGINT handler for state machine" << endl;
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler  for SIGTERM signal - Used for State Machine finishing
        */
        if (signal(SIGTERM, sigHandlerStateMachine) == SIG_ERR ){
          //cout <<  "Failed to set SIGINT handler for state machine" << endl;
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler  for SIGUSR1 signal. This signal will be used for State Machine start.
        */
        if( signal(SIGUSR1, sigHandlerStateMachine) == SIG_ERR){
          //cout <<  "Failed to set SIGUSR1 handler for state machine" << endl;
          _exit(EXIT_FAILURE);
        }

        /*
        * Initialize daemon mode parameters
        */
        if(daemon_mode){

          if (setsid() == -1) /* Become leader of new session */
            return -1;

        }

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Create child");
        logger::set_level(dlevel);
        /*
        * Create PI Robot instance
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create hardware support");
        std::shared_ptr<pirobot::PiRobot> pirobot(new pirobot::PiRobot());
        pirobot->set_configuration(robot_conf);

        //Create State factory for State Machine
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create State Factory support");
        std::shared_ptr<weather::WeatherStFactory> factory(new weather::WeatherStFactory(firstState));

        std::shared_ptr<mqqt::MqqtItf> clMqqt;
        if(mqtt){
          logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "MQQT detected");
          try{
            int res = mosqpp::lib_init();
            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "MQQT library intialized Res: " + std::to_string(res));

            // Load MQQT server configuration
            mqqt::MqqtServerInfo info = mqqt::MqqtServerInfo::load(mqqt_conf);
            clMqqt = std::shared_ptr<mqqt::MqqtItf>(new mqqt::MqqtClient<mqqt::MosquittoClient>(info));
            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "MQQT configuration loaded");
          }
          catch(std::runtime_error& rterr){
            logger::log(logger::LLOG::ERROR, "main", std::string(__func__) +
                "Could not load MQQT configuration configuration " + robot_conf + " Error: " + rterr.what());
            _exit(EXIT_FAILURE);
          }
        }

        /*
        * Create State machine
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create state machine.");
        stm = std::shared_ptr<smachine::StateMachine>(new smachine::StateMachine(factory, pirobot, clMqqt));

        /*
        * Web interface for settings and status
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Created Web interface");
        std::shared_ptr<weather::web::WebWeather> web(new weather::web::WebWeather(8080, stm));
        web->http::web::WebSettings::start();

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Waiting for State Machine finishing");
        stm->wait();

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "State machine finished");
        web->http::web::WebSettings::stop();

        sleep(2);
        stm.reset();

        if(mqtt){
          mosqpp::lib_cleanup();
        }

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Child finished");
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

      /*
      *
      */
      if(!daemon_mode){
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
        cout <<  "Weather project finished" << endl;
      }
  }

  exit(EXIT_SUCCESS);
}

