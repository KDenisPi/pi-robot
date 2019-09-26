#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "tsl2561.h"


using pi2c = std::shared_ptr<pirobot::i2c::I2C>;
using ptsl2561 = std::shared_ptr<pirobot::item::Tsl2561>;

/*
* I2C TSL2561 test.
* Precondition: Connect TSL2561 sensor
* 1. Connect to default I2C address
* 2. Get value durind 10 seconds
*/


using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;

    std::cout << "Started " << std::endl;

    std::cout << "Create provider" << std::endl;
    pi2c p_pi2c = std::make_shared<pirobot::item::Tsl2561>();

    ptsl2561 p_ptsl2561 = std::make_shared<pirobot::i2c::I2C>("TSL2561", p_pi2c);


    std::cout << "Release TSL2561 objects " << std::endl;
    p_ptsl2561.reset();
    std::cout << "Release Provider objects " << std::endl;
    p_pi2c.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
