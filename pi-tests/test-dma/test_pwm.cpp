#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "core_pwm.h"

using namespace std;


/*
* Simple DMA test.
* Copy memory buffer to another one using DMA 10
*
*/
int main (int argc, char* argv[])
{
    bool success = true;

    std::cout << "Starting..." << std::endl;
    pi_core::core_pwm::PwmCore* pwm = new  pi_core::core_pwm::PwmCore();

    success = pwm->Initialize();
    delete pwm;
    std::cout << "Finished " << success << std::endl;

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
