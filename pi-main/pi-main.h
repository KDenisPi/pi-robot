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
#include <fcntl.h>

#include "WebSettings.h"
#include "StateMachine.h"
#include "logger.h"

#define BD_MAX_CLOSE  8192

namespace pimain {

template<class F, class W>
class PiMain {
public:
    PiMain(const std::string& project_name, const std::string& logs_dir = "/var/log") : _proj_name(project_name), _log_folder(logs_dir) {
        _project_log =  _log_folder + "/" + _proj_name + ".log";
        _project_err =  _log_folder + "/" + _proj_name + ".err";
    }

    virtual ~PiMain() {}

    /*
    * Singnal handler for State Machine
    */
    static void sigHandlerStateMachine(int sign){
        std::cout <<  "Detected signal " << sign  << std::endl;
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
        std::cout <<  "Parent: Detected signal " << sign  << std::endl;

        if(_stmPid){
            std::cout <<  "Parent: Send signal to child  " << sign  << std::endl;
            kill(_stmPid, sign);
        }
    }

    const char* err_message = "Error. No configuration file.";
    const char* help_message = "app --conf cfg_file [--mqtt-conf mqtt_cfg] [--nodaemon] [--llevel INFO|DEBUG|NECECCARY|ERROR] [--debug] [--fstate FirstStateName]";

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
    const std::string conf_main() const {
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
    * Debug mode
    */
    void set_debug_mode(const bool debug_mode) {
       _debug_mode = debug_mode;
    }

    const bool debug_mode() const {
       return _debug_mode;
    }

    /*
    * Use HTTP module
    */
    void set_use_http(const bool use_http) {
       _use_http = use_http;
    }

    const bool use_http() const {
       return _use_http;
    }


    /*
    * Load configuration parameters
    */
    void load_configuration(const int argc, char* argv[]){
        for(int i = 0; i < argc; i++){
            std::string arg = argv[i];
            std::cout <<  "Arg: " << i << " [" << arg << "]" << std::endl;

            if(strcmp(argv[i], "--conf") == 0){
                _robot_conf = _validate_file_parameter(++i, argc, argv);
            }
            else if(strcmp(argv[i], "--mqtt-conf") == 0){
                _mqtt_conf = _validate_file_parameter(++i, argc, argv);
                _use_mqtt = true;
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
            else if(strcmp(argv[i], "--nohttp") == 0){
                _use_http = false;
            }
            else if(strcmp(argv[i], "--debug") == 0){
                _debug_mode = true;
            }
        }
    }

    /*
    * Validate configuration
    */
    void validate_configuration() {
        if(_robot_conf.empty()){
            std::cout <<  err_message << std::endl <<  help_message << std::endl;
            _exit(EXIT_FAILURE);
        }
    }

    /*
    * Run application
    */
    void run(){
        if(debug_mode()){
            run_single();
        }
        else if(daemon_mode()){
            run_daemon();
        }
        else
        {
            run_child();
        }

    }

    void create_factory(const std::string& firstState) {
        if(!_factory)
            _factory = std::make_shared<F>(firstState);
    }

    const std::shared_ptr<smachine::StateFactory> factory(){
        return _factory;
    }

    /*
    *
    */
   void create_web(const uint16_t port, std::shared_ptr<smachine::StateMachineItf> itf){
       if(!_web)
           _web = std::shared_ptr<W>();
   }

   const std::shared_ptr<http::web::WebSettingsItf> web(){
       return _web;
   }

private:

    const char* str_null = "/dev/null";

    std::string _proj_name;
    std::string _log_folder;
    std::string _project_log;
    std::string _project_err;

    const int web_port = 8080;
    /*
    * Run application in daemon mode
    */
    void run_daemon(){

        switch(_stmPid = fork()){
        case -1:
            std::cerr <<  "Could not create state machine application" << std::endl;
            _exit(EXIT_FAILURE);

        case 0: //child
            //initialize daemon configuration
            daemon_initialization();

            //Initilize signal handlers
            initialize_signal_handlers();

            //Initialize and run
            initilize_and_run();

            //Initilize signal handlers
            initialize_signal_handlers();

            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Waiting for State Machine finishing");
            _stm->wait();

            finish();

            _exit(EXIT_SUCCESS);
             break;

        default: //parent
            sleep(2);
            //send command to start
            kill(_stmPid, SIGUSR1);
        }
    }

    /*
    * Run child process not in daemon mode
    */
   void run_child(){
        switch(_stmPid = fork()){
        case -1:
            std::cerr <<  "Could not create state machine application" << std::endl;
            _exit(EXIT_FAILURE);

        case 0: //child
            //Initialize and run
            initilize_and_run();

            //Initilize signal handlers
            initialize_signal_handlers();

            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Waiting for State Machine finishing");
            _stm->wait();

            finish();

            _exit(EXIT_SUCCESS);
             break;

        default: //parent
            sleep(2);
            //send command to start
            kill(_stmPid, SIGUSR1);
        }
   }

    void run_single() {
        //Initialize and run
        initilize_and_run();

        //Initilize signal handlers
        initialize_signal_handlers();

        _stmPid = getpid();

        sleep(2); //start State Machine
        //_stm->run();
        //send command to start
        kill(_stmPid, SIGUSR1);

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Waiting for State Machine finishing");
        _stm->wait();

        finish();
    }

    /*
    * Initialize objects and wait
    */
   void initilize_and_run(){
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Create child. LLEVEL: " + std::to_string(_llevel));
        logger::set_level(_llevel);
        /*
        * Create PI Robot instance
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create hardware support");
        _pirobot = std::make_shared<pirobot::PiRobot>();
        _pirobot->set_configuration(_robot_conf);

        //Create State factory for State Machine
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create State Factory support");
        create_factory(_firstState);
        factory()->set_configuration(_robot_conf);

        /*
        * Create State machine
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create state machine.");
        _stm = std::make_shared<smachine::StateMachine>(_factory, _pirobot);

        /*
        * Web interface for settings and status
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Created Web interface. Use HTTP: " + std::to_string(use_http()));
        if(use_http()){
            create_web(web_port, _stm);
            if(web()){
                web()->http::web::WebSettingsItf::start();
            }
        }
   }

    /*
    * Daemon initialization
    */
   void daemon_initialization(){
        /*
        * Become leader of new session
        */
        if (setsid() == -1)
            _exit(EXIT_FAILURE);

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
        for (int fd = 0; fd < maxfd; fd++){
          close(fd);
        }

        /*
        * Reopen stdin, stdout, stderr
        */
        _fd_null = open(str_null, O_RDWR);
        if (_fd_null != STDIN_FILENO) /* 'fd' should be 0 */
          _exit(EXIT_FAILURE);

        _fd_log = open(_project_log.c_str(), O_RDWR|O_CREAT|O_APPEND, 0666);
        if (_fd_log != STDOUT_FILENO) /* 'fd' should be 1 */
          _exit(EXIT_FAILURE);

        _fd_err = open(_project_err.c_str(), O_RDWR|O_CREAT|O_APPEND, 0666);
        if (_fd_err != STDERR_FILENO) /* 'fd' should be 2 */
          _exit(EXIT_FAILURE);
   }

    /*
    * Initilize handlers
    */
   void initialize_signal_handlers() {
        /*
        * Add handler for SIGALARM signal (used for State Machine Timers)
        */
        ADD_SIGNAL(SIGALRM)


        //std::function<void(int)> hnd = std::bind(&PiMain::sigHandlerStateMachine, this, std::placeholders::_1);
        /*
        * Add handler  for SIGUSR2 signal - Used for State Machine finishing
        */
        if (signal(SIGUSR2, PiMain::sigHandlerStateMachine) == SIG_ERR ){
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler for SIGTERM signal - Used for State Machine finishing
        */
        if (signal(SIGTERM, PiMain::sigHandlerStateMachine) == SIG_ERR ){
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler for SIGINT signal - Used for State Machine finishing
        */
        if (signal(SIGINT, PiMain::sigHandlerStateMachine) == SIG_ERR ){
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler for SIGQUIT signal - Used for State Machine finishing
        */
        if (signal(SIGQUIT, PiMain::sigHandlerStateMachine) == SIG_ERR ){
          _exit(EXIT_FAILURE);
        }

        /*
        * Add handler  for SIGUSR1 signal. This signal will be used for State Machine start.
        */
        if( signal(SIGUSR1, PiMain::sigHandlerStateMachine) == SIG_ERR){
          _exit(EXIT_FAILURE);
        }
    }

    /*
    * Finish processing and release objects
    */
    void finish(){

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "State machine finished");
        if(_web)
            _web->http::web::WebSettingsItf::stop();

        sleep(2);
        _stm.reset();
        _factory.reset();
        _pirobot.reset();

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Child finished");
        logger::release();
    }

    /*
    *
    */
    std::string _validate_file_parameter(const int idx, const int argc, char* argv[]){
        std::string filename;
        if(idx == argc){
            std::cout <<  err_message << std::endl <<  help_message << std::endl;
            _exit(EXIT_FAILURE);
        }

        filename = argv[idx];
        std::ifstream file_stream(filename);
        if(!file_stream){
            std::cout <<  "Configuration file " << filename << " does not exist or not available." << std::endl;
            _exit(EXIT_FAILURE);
        }

        file_stream.close();
        return filename;
    }

    public:
    /*
    * State Machine instance
    */
    static std::shared_ptr<smachine::StateMachine> _stm;

    /*
    * State Machine process ID
    */
    static pid_t _stmPid;

    private:
    /*
    * Daemon flag
    */
    bool _daemon_mode = true;
    /*
    * Debug flag
    */
    bool _debug_mode = false;

    /*
    * Default debug level
    */
    logger::LLOG _llevel = logger::LLOG::DEBUG;

    /**
     * Use HTTP module or not
     */
    bool _use_http = true;

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

    int _fd_log;
    int _fd_null;
    int _fd_err;

protected:
    std::shared_ptr<pirobot::PiRobot> _pirobot;
    std::shared_ptr<smachine::StateFactory> _factory;
    std::shared_ptr<http::web::WebSettingsItf> _web;
};

template<class F, class W>
pid_t pimain::PiMain<F, W>::_stmPid;

template<class F, class W>
std::shared_ptr<smachine::StateMachine> pimain::PiMain<F, W>::_stm;

} //namespace pimain
#endif
