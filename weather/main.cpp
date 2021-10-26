#include <fstream>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#include "version.h"
#include "defines.h"

#include "StateMachine.h"
#include "context.h"
#include "WeatherStFactory.h"
#include "WebSettingsWeather.h"


using namespace std;

#define BD_MAX_CLOSE  8192

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
logger::LLOG llevel = logger::LLOG::INFO;

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

  //
  // Stop state machine
  //
  if(sign == SIGUSR2  || sign == SIGTERM || sign == SIGQUIT || sign == SIGINT) {
    stm->finish();
  }
  else if( sign == SIGHUP ){//Reload configuration - debug level only for now
    logger::set_update_conf();
  }
  else if (sign == SIGUSR1){// Start state machine
    stm->run();
  }
}

/*
* Singnal handler for parent
*/
static void sigHandlerParent(int sign){
  cout <<  "Parent: Detected signal " << sign  << endl;
  if(stmPid){
    cout <<  "Parent: Send signal to child  " << sign  << endl;
    kill(stmPid, sign);
  }
}

const char* err_message = "Error. No configuration file.";
const char* help_message = "weather --conf cfg_file [--mqtt-conf mqtt_cfg] [--nodaemon] [--llevel INFO|DEBUG|NECECCARY|ERROR] [--fstate FirstStateName]";

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
* program --conf path_to_configuration [--mqtt-conf mqtt_configuration_file]
*    --mqtt - optional if MQTT client should be used
*/
int main (int argc, char* argv[])
{
  bool mqtt = false;
  std::string robot_conf, mqtt_conf;
  stmPid  = 0;
  sigset_t new_set;

  cout <<  "Weather started" << endl;

  for(int i = 0; i < argc; i++){
    std::string arg = argv[i];
    cout <<  "Arg: " << i << " [" << arg << "]" << endl;

    if(strcmp(argv[i], "--conf") == 0){
      robot_conf = validate_file_parameter(++i, argc, argv);
    }
    else if(strcmp(argv[i], "--mqtt-conf") == 0){
      mqtt_conf = validate_file_parameter(++i, argc, argv);
      mqtt = true;
    }
    else if(strcmp(argv[i], "--nodaemon") == 0){
      daemon_mode = false;
    }
    else if(strcmp(argv[i], "--llevel") == 0){
      if(++i < argc){
        llevel = logger::Logger::type_by_string( argv[i] );
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

  /*
  * Initialize daemon mode parameters
  */
  if(daemon_mode){

    switch (fork()) {
      case -1: return -1;
      case 0: break;
      default: _exit(EXIT_SUCCESS);
    }

    /*
    * Become leader of new session
    */
    if (setsid() == -1)
      _exit(EXIT_FAILURE);
  }

  switch(stmPid = fork()){
    case -1:
      std::cerr <<  "Could not create state machine application" << endl;
        _exit(EXIT_FAILURE);

    case 0: //child
      {
      /*
      * Initialize daemon mode parameters
      */
      if(daemon_mode){
        int fd;
        /*
        * Clear the process umask (Section 15.4.6), to ensure that, when the daemon creates
        * files and directories, they have the requested permissions.
        */
        umask(0);

        /*
        * Change the process’s current working directory
        */
        if ( chdir("/") == -1 )
          _exit(EXIT_FAILURE);

        /*
        * Close all opened file descriptors
        */
        int maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1) /* Limit is indeterminate... */
          maxfd = BD_MAX_CLOSE;
        /* so take a guess */
        for (fd = 0; fd < maxfd; fd++){
          close(fd);
        }

        /*
        * Reopen stdin, stdout, stderr
        */
        fd = open("/dev/null", O_RDWR);
        if (fd != STDIN_FILENO) /* 'fd' should be 0 */
          _exit(EXIT_FAILURE);

        fd = open("/var/log/weather.log", O_RDWR|O_CREAT|O_APPEND, 0666);
        if (fd != STDOUT_FILENO) /* 'fd' should be 1 */
          _exit(EXIT_FAILURE);

        fd = open("/var/log/weather.err", O_RDWR|O_CREAT|O_APPEND, 0666);
        if (fd != STDERR_FILENO) /* 'fd' should be 2 */
          _exit(EXIT_FAILURE);
      }
        /*
        * Add handler for SIGALARM signal (used for State Machine Timers)
        */
        ADD_SIGNAL(SIGALRM)

        /*
        * Add handler  for SIGUSR2 signal - Used for State Machine finishing
        */
        if (signal(SIGUSR2, sigHandlerStateMachine) == SIG_ERR ){
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler for SIGTERM signal - Used for State Machine finishing
        */
        if (signal(SIGTERM, sigHandlerStateMachine) == SIG_ERR ){
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler for SIGINT signal - Used for State Machine finishing
        */
        if (signal(SIGINT, sigHandlerStateMachine) == SIG_ERR ){
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler for SIGQUIT signal - Used for State Machine finishing
        */
        if (signal(SIGQUIT, sigHandlerStateMachine) == SIG_ERR ){
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler  for SIGUSR1 signal. This signal will be used for State Machine start.
        */
        if( signal(SIGUSR1, sigHandlerStateMachine) == SIG_ERR){
          _exit(EXIT_FAILURE);
        }

        mytime("Started");

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Create child");
        logger::set_level(llevel);
        /*
        * Create PI Robot instance
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create hardware support");
        std::shared_ptr<pirobot::PiRobot> pirobot = std::make_shared<pirobot::PiRobot>();
        pirobot->set_configuration(robot_conf);

        //Create State factory for State Machine
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create State Factory support");
        std::shared_ptr<weather::WeatherStFactory> factory = std::make_shared<weather::WeatherStFactory>(firstState);
        factory->set_configuration(robot_conf);

        /*
        * Create State machine
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create state machine.");
        stm = std::make_shared<smachine::StateMachine>(factory, pirobot);

        /*
        * Web interface for settings and status
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Created Web interface");
        std::shared_ptr<smachine::StateMachineItf> itf = std::dynamic_pointer_cast<smachine::StateMachineItf>(stm);
        std::shared_ptr<weather::web::WebWeather> web = std::make_shared<weather::web::WebWeather>(8080, itf);
        web->http::web::WebSettings::start();

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Waiting for State Machine finishing");
        stm->wait();

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "State machine finished");
        web->http::web::WebSettings::stop();

        sleep(2);
        stm.reset();

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Child finished");
        logger::release();

        _exit(EXIT_SUCCESS);
      }
      break;

    default: //parent

      sleep(2);

      //send command to start
      kill(stmPid, SIGUSR1);

      /*
      *
      */
      if(!daemon_mode){
        std::cout <<  "State machine created " <<  stmPid << endl;
        if( signal(SIGUSR1, sigHandlerParent) == SIG_ERR){
          std::cerr <<  "Parent handler error " <<  stmPid << endl;
        }

        if (signal(SIGINT, sigHandlerParent) == SIG_ERR ){
          std::cerr <<  "Parent handler error " <<  stmPid << endl;
          kill(stmPid, SIGINT);
        }
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

