#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "logger.h"
#include "Timer.h"
#include "Timers2.h"

void handler(std::shared_ptr<smachine::Event> event);

void handler(std::shared_ptr<smachine::Event> event){
    std::cout << "Detected timer event ID: " <<  event->id() << " Type: " << event->type() << std::endl;
}

int main (int argc, char* argv[])
{
    std::cout << "Test timers started..." << std::endl;

    logger::log_init(std::string("test-timer.log"));
    logger::log(logger::LLOG::DEBUG, "TIMER", std::string(__func__) + " Started ");

    std::cout << "PID: " <<  gettid() << std::endl;

    sigset_t new_set, org_set;
    sigemptyset (&new_set);
    sigaddset (&new_set, TIMER_SIG);
    pthread_sigmask(SIG_BLOCK, &new_set, &org_set);

    smachine::timer::TimersPtr tmrs = std::make_shared<smachine::timer::Timers>();
    if(tmrs){
        tmrs->put_event = std::bind(&handler, std::placeholders::_1);

        tmrs->start();
        sleep(1);
        smachine::timer::timer_info tm_info = {100,2,0,true};

        std::cout << "Create timer" << std::endl;
        tmrs->create_timer(tm_info);
        tm_info = {101,1,0,false};
        tmrs->create_timer(tm_info);
        sleep(3);

        tmrs->reset_timer(100);
        sleep(3);

        tmrs->stop();
        std::cout << "Finished" << std::endl;
    }

    exit(EXIT_SUCCESS);
}