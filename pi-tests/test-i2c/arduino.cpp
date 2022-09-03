#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "I2C.h"

using pi2c = std::shared_ptr<pirobot::i2c::I2C>;

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;

    logger::log_init("/var/log/pi-robot/arduino_log");
    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Arduino");

    std::cout << "Create provider" << std::endl;
    pi2c p_pi2c = std::make_shared<pirobot::i2c::I2C>();
    
    std::cout << "Release Provider objects " << std::endl;
    p_pi2c.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}