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

#include "logger.h"
#include "WebSettings.h"
#include "timers.h"
#include "StateMachine.h"

#define BD_MAX_CLOSE  8192

using Rob = pirobot::PiRobot;

namespace pimain {

template<class F, class E, class W>
class PiMain {
public:
    PiMain(const std::string& project_name, const std::string& logs_dir = "/var/log") : _proj_name(project_name), _log_folder(logs_dir) {
        _project_log =  _log_folder + "/" + _proj_name + ".log";
        _project_err =  _log_folder + "/" + _proj_name + ".err";

        piutils::timers::Timers::add_signal(SIGALRM);
    }

    virtual ~PiMain() {}

    /*
    * Singnal handler for State Machine
    */
    static void sigHandlerStateMachine(int sign){
        ////std::cout  "Detected signal " << sign  << std::endl;
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
            ////std::cout  "Detected signal. Run service " << sign  << std::endl;
            _stm->run();
        }
    }

    /*
    * Singnal handler for parent
    */
    static void sigHandlerParent(int sign){
        ////std::cout  "Parent: Detected signal " << sign  << std::endl;

        if(_stmPid){
            ////std::cout  "Parent: Send signal to child  " << sign  << std::endl;
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
            ////std::cout  "Arg: " << i << " [" << arg << "]" << std::endl;

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

        validate_configuration();
    }

    /*
    * Validate configuration
    */
    void validate_configuration() {
        if(_robot_conf.empty()){
            ////std::cout  err_message << std::endl <<  help_message << std::endl;
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

    const std::shared_ptr<F> factory(){
        return _factory;
    }

   const std::shared_ptr<W> web(){
       return _web;
   }

    std::shared_ptr<E> environment(){
        return _env;
    }

    std::shared_ptr<Rob> robot(){
        return _pirobot;
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
            logger::release();
            //std::cout  << "--- start after fork daemon ---" << get_log_filename() << std::endl;

            //initialize daemon configuration
            daemon_initialization();

            logger::log_init(get_log_filename());
            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " After fork (daemon)");

            //Initialize and run
            if(initilize_and_run()){
                //Initilize signal handlers
                initialize_signal_handlers();

                logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Waiting for State Machine finishing");
                _stm->wait();
            }

            finish();

            logger::finish();
            logger::release();

            _exit(EXIT_SUCCESS);
             break;

        default: //parent
            sleep(5);
            //send command to start
            kill(_stmPid, SIGUSR1);

            logger::finish();
            logger::release();

            _exit(EXIT_SUCCESS);
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
            ////std::cout  "--- start after fork ---" << std::endl;
            logger::release();
            logger::log_init(get_log_filename());

            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " After fork");
            //Initialize and run
            initilize_and_run();

            //Initilize signal handlers
            initialize_signal_handlers();

            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Waiting for State Machine finishing");
            _stm->wait();

            ////std::cout  "--- finish---" << std::endl;
            finish();

            ////std::cout  "--- after finish---" << std::endl;

            logger::finish();
            logger::release();

            _exit(EXIT_SUCCESS);
             break;

        default: //parent
            sleep(5);
            //send command to start
            kill(_stmPid, SIGUSR1);

            logger::finish();
            logger::release();
            _exit(EXIT_SUCCESS);
        }
   }

    void run_single() {

        logger::log_init(get_log_filename());

        //Initialize and run
        if(initilize_and_run()){
            //Initilize signal handlers
            initialize_signal_handlers();

            _stmPid = getpid();

            sleep(5); //start State Machine
            //_stm->run();
            //send command to start
            kill(_stmPid, SIGUSR1);

            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Waiting for State Machine finishing");
            _stm->wait();

        }

        finish();

        logger::finish();
        logger::release();
    }

    /*
    * Initialize objects and wait
    */
   bool initilize_and_run(){
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Create child. LLEVEL: " + std::to_string(_llevel));
        logger::set_level(_llevel);
        /*
        * Create PI Robot instance
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create hardware support");
        _pirobot = std::make_shared<Rob>();
        _pirobot->set_configuration(_robot_conf);

        //Create State factory for State Machine
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create State Factory support");
        _factory = std::make_shared<F>(_firstState);
        _factory->set_configuration(_robot_conf);

        //Create Environment
        _env = std::make_shared<E>();

        /*
        * Create State machine
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Create state machine.");
        _stm = std::make_shared<smachine::StateMachine>();

        _stm->get_robot = std::bind(&PiMain<F,E,W>::robot, this);

        _stm->getfirststate = std::bind(&F::get_first_state, _factory);
        _stm->get_state = std::bind(&F::get_state, _factory, std::placeholders::_1);
        _stm->configure_environment = std::bind(&E::configure, _env, std::placeholders::_1);

        _stm->init();

        /*
        * Web interface for settings and status
        */
        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "Created Web interface. Use HTTP: " + std::to_string(use_http()));
        if(use_http()){
           _web = std::make_shared<W>(_env->get_web_port());
           _web->set_web_root(_env->get_web_root());
           _web->add_dir_map("data", _env->get_csv_data());
           _web->add_dir_map("json", _env->get_json_data());

            if(web()){
                web()->http::web::WebSettingsItf::start();
            }
        }

        return true;
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

       /*
        Unhandled stop signals can generate EINTR for some Linux system calls
        On Linux, certain blocking system calls can return EINTR even in the absence of a
        signal handler. This can occur if the system call is blocked and the process is
        stopped by a signal (SIGSTOP, SIGTSTP, SIGTTIN, or SIGTTOU), and then resumed by delivery of a SIGCONT signal.
        The following system calls and functions exhibit this behavior: epoll_pwait(),
        epoll_wait(), read() from an inotify file descriptor, semop(), semtimedop(), sigtimedwait(),
        and sigwaitinfo().
        */

        if (signal(SIGALRM, PiMain::sigHandlerStateMachine) == SIG_ERR ){
          _exit(EXIT_FAILURE);
        }


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

        ////std::cout  "--- main finish---" << std::endl;

        if(_web)
            _web->http::web::WebSettingsItf::stop();

        sleep(2);
        _stm.reset();
        ////std::cout  "--- main STM finished---" << std::endl;
        _factory.reset();
        ////std::cout  "--- main factory finished---" << std::endl;
        _pirobot.reset();
        ////std::cout  "--- main robot finished---" << std::endl;

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Child finished");
        //logger::release();
    }

    /*
    *
    */
    std::string _validate_file_parameter(const int idx, const int argc, char* argv[]){
        std::string filename;
        if(idx == argc){
            ////std::cout  err_message << std::endl <<  help_message << std::endl;
            _exit(EXIT_FAILURE);
        }

        filename = argv[idx];
        std::ifstream file_stream(filename);
        if(!file_stream){
            ////std::cout  "Configuration file " << filename << " does not exist or not available." << std::endl;
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

    /**
     * @brief
     *
     */
    static std::shared_ptr<E> _env;


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

    const std::string get_log_filename() const {
        if(_debug_mode) return log_folder + log_single;
        if(_daemon_mode) return log_folder + log_daemon;

        return log_folder+log_fork;
    }

protected:
    std::shared_ptr<Rob> _pirobot;
    std::shared_ptr<F> _factory;
    std::shared_ptr<W> _web;

    const std::string log_folder = "/var/log/pi-robot/";
    const std::string log_single = "async_log";
    const std::string log_fork = "async_client_log";
    const std::string log_daemon = "async_daemon_log";
};

template<class F, class E, class W>
pid_t pimain::PiMain<F, E, W>::_stmPid;

template<class F, class E, class W>
std::shared_ptr<smachine::StateMachine> pimain::PiMain<F, E, W>::_stm;

template<class F, class E, class W>
std::shared_ptr<E> pimain::PiMain<F, E, W>::_env;

} //namespace pimain
#endif
