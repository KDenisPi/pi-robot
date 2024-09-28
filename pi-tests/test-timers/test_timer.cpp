#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "logger.h"
#include "Timer.h"

int main (int argc, char* argv[])
{
    std::cout << "Test timers started..." << std::endl;

    logger::log_init(std::string("test-timer.log"));
    logger::log(logger::LLOG::DEBUG, "TIMER", std::string(__func__) + " Started ");

    smachine::timer::TimerPtr tmr = smachine::timer::Timer::create(100);

    exit(EXIT_SUCCESS);
}