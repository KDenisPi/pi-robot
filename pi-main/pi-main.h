/*
 * pi-mail.h
 *
 * Main class for application creating
 *
 * Created on: Jan 08, 2020
 *      Author: Denis Kudia
 */

#ifndef PI_MAIN_APPLICATION_
#define PI_MAIN_APPLICATION_

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

#include "logger.h"

#define BD_MAX_CLOSE  8192

namespace pimain {

class PiMain {
public:
    PiMain() {}
    virtual ~PiMain() {}

    /*
    * Singnal handler for State Machine
    */
    static void sigHandlerStateMachine(int sign){
        //
        // Stop state machine
        //
        if(sign == SIGUSR2  || sign == SIGTERM || sign == SIGQUIT || sign == SIGINT) {
            _stm->finish();
        }
        else if( sign == SIGHUP ){//Reload configuration - debug level only for now
            logger::set_update_conf();
        }
        else if (sign == SIGUSR1){// Start state machine
            _stm->run();
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
    void set_first_state(const std::string& first_state){
        _firstState = first_state;
    }

    const std::string first_state() const {
        return _firstState;
    }

    /*
    *
    */
    void set_llevel(const logger::LLOG llev){
       _llevel = llev;
    }

    const logger::LLOG llevel() const {
       return _llevel;
    }

    /*
    *
    */
    const std:string conf_main() const {
        return _robot_conf;
    }

    void set_conf_main(const std::string& main_conf){
        _robot_conf = main_conf;
    }

    /*
    * Daemon mode
    */
    void set_daemon_mode(const bool daemon_mode) {
       _daemon_mode = daemon_mode;
    }

    const bool daemon_mode() const {
       return _daemon_mode;
    }

    /*
    * Load configuration parameters
    */
    void load_configuration(const int argc, char* argv[]){
        for(int i = 0; i < argc; i++){
            std::string arg = argv[i];
            cout <<  "Arg: " << i << " [" << arg << "]" << endl;

            if(strcmp(argv[i], "--conf") == 0){
                _robot_conf = _validate_file_parameter(++i, argc, argv);
            }
            else if(strcmp(argv[i], "--mqtt-conf") == 0){
                _mqtt_conf = _validate_file_parameter(++i, argc, argv);
                _mqtt = true;
            }
            else if(strcmp(argv[i], "--nodaemon") == 0){
                _daemon_mode = false;
            }
            else if( (strcmp(argv[i], "--llevel") == 0) && (++i < argc)){
                    _llevel = logger::Logger::type_by_string( argv[i] );
            }
            else if( (strcmp(argv[i], "--fstate") == 0) && (++i < argc)){
                    _firstState = argv[i];
            }
        }
    }

    /*
    * Validate configuration
    */
    void validate_configuration() {
        if(robot_conf.empty()){
            cout <<  err_message << endl <<  help_message << endl;
            _exit(EXIT_FAILURE);
        }
    }

private:

    /*
    *
    */
    std::string _validate_file_parameter(const int idx, const int argc, char* argv[]){
        std::string filename;
        if(idx == argc){
            cout <<  err_message << endl <<  help_message << endl;
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
* State Machine instance
*/
std::shared_ptr<smachine::StateMachine> _stm;

/*
* State Machine process ID
*/
pid_t _stmPid;

/*
* Daemon flag
*/
bool _daemon_mode = true;

/*
* Default debug level
*/
logger::LLOG _llevel = logger::LLOG::INFO;

/*
* First state
*/
std::string _firstState = "StInitialization";

/*
*
*/
std::string _robot_conf;    //main configuration file
bool _use_mqtt = false;     //use MQTT flag
std::string _mqtt_conf;     //MQTT configuration file


};

} //namespace pimain
#endif
