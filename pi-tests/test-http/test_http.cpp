#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <memory>

#include "logger.h"
#include "web_settings_test.h"

using namespace std;

//Set to 1 by the signal handler when CTRL+C (SIGINT) or SIGTERM is received
static volatile sig_atomic_t stop_flag = 0;

static void signal_handler(int signal){
    stop_flag = 1;
}

//Change the working directory to the folder that contains the executable so
//that relative paths (web root, data/json maps) resolve regardless of where
//the binary was launched from.
static bool chdir_to_exe_dir(){
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if(len < 0){
        std::cerr << "Cannot resolve executable path: " << std::strerror(errno) << std::endl;
        return false;
    }
    exe_path[len] = '\0';

    const char* dir = dirname(exe_path); //dirname may modify exe_path
    if(chdir(dir) != 0){
        std::cerr << "Cannot change directory to " << dir << ": " << std::strerror(errno) << std::endl;
        return false;
    }

    std::cout << "Working directory: " << dir << std::endl;
    return true;
}

int main (int argc, char* argv[])
{
    bool success = true;

    //Resolve relative paths (web root, data/json maps) against the executable location
    chdir_to_exe_dir();

    auto web_srv = std::make_shared<http_test::HttpTestWebSet>();
    auto app_context = std::make_shared<http_test::Context>();


    web_srv->set_web_root(".");
    web_srv->set_default_page("default.html");

    web_srv->add_dir_map("statis", "./static");
    web_srv->add_dir_map("data", "./conf");
    web_srv->add_dir_map("json", "./conf");

    logger::log_init(std::string("test-http.log"));
    logger::log(logger::LLOG::DEBUG, "HTTP", std::string(__func__) + " Create Web server ");
    std::cout << "Create Web server " << std::endl;

    //Handle CTRL+C (SIGINT) and SIGTERM so the server can shut down gracefully
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    web_srv->start();

    std::cout << "Web server is running. Press CTRL+C to stop." << std::endl;

    //Run until CTRL+C (or SIGTERM) is received
    while(!stop_flag){
        sleep(1);
    }

    std::cout << std::endl << "Release Web server " << std::endl;

    web_srv->stop();
    web_srv.reset();

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
