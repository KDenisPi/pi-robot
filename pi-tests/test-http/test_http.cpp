#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "web_settings_test.h"

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;
    std::unique_ptr<http_test::HttpTestWebSet> web_srv = std::make_unique<http_test::HttpTestWebSet>();
    std::shared_ptr<http_test::Context> app_context = std::make_shared<http_test::Context>();


    web_srv->set_web_root("/home/deniskudia/sources/pi-robot/pi-tests/test-http");
    web_srv->set_default_page("default.html");

    web_srv->add_dir_map("statis", "./static");
    web_srv->add_dir_map("data", "/home/deniskudia/sources/pi-robot/conf");
    web_srv->add_dir_map("json", "/home/deniskudia/sources/pi-robot/conf");

    logger::log_init(std::string("test-http.log"));
    logger::log(logger::LLOG::DEBUG, "HTTP", std::string(__func__) + " Create Web server ");
    std::cout << "Create Web server " << std::endl;

    web_srv->start();

    sleep(15);

    std::cout << "Release Web server " << std::endl;

    web_srv->stop();
    web_srv.release();

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
