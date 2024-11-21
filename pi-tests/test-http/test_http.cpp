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

    logger::log_init(std::string("test-http.log"));
    logger::log(logger::LLOG::DEBUG, "HTTP", std::string(__func__) + " Create Web server ");
    std::cout << "Create Web server " << std::endl;

    web_srv->start();

    sleep(10);

    std::cout << "Release Web server " << std::endl;

    web_srv->stop();
    web_srv.release();

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
