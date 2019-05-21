#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "core_pwm.h"

using namespace std;


/*
* Simple PWM & PWM Clock test.
* Start PWM object and configura PWM Clock
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
