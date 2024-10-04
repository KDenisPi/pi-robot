#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "logger.h"
#include "Timer.h"
#include "Timers2.h"

int main (int argc, char* argv[])
{
    std::cout << "Test timers started..." << std::endl;

    logger::log_init(std::string("test-timer.log"));
    logger::log(logger::LLOG::DEBUG, "TIMER", std::string(__func__) + " Started ");

    smachine::timer::TimersPtr tmrs = std::make_shared<smachine::timer::Timers>();
    smachine::timer::TimerPtr tmr = smachine::timer::Timer::create(100);

    if(tmrs){
        tmrs->start();

        if(tmr){
            std::cout << "Start timer" << std::endl;
            tmr->start();
            sleep(5);
            std::cout << "Stop timer" << std::endl;
            tmr->stop();
            std::cout << "Release timer" << std::endl;
            tmr.reset();
        }
        else
            std::cout << "Failed create timer" << std::endl;
    }

    exit(EXIT_SUCCESS);
}