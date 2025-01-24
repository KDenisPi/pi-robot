#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "logger.h"
#include "Timer.h"
#include "Timers2.h"

void handler(std::shared_ptr<smachine::Event> event);
int tm_counters[3] = {0,0,0};


void handler(std::shared_ptr<smachine::Event> event){
    if(100 <= event->id())
        tm_counters[event->id()-100]++;
    //std::cout << "Detected timer event ID: " <<  event->id() << " Type: " << event->type() << std::endl;
}

int main (int argc, char* argv[])
{
    std::cout << "Test timers started..." << std::endl;

    logger::log_init(std::string("test-timer.log"));
    logger::log(logger::LLOG::DEBUG, "TIMER", std::string(__func__) + " Started ");

    std::cout << "PID: " <<  gettid() << std::endl;

/*
    sigset_t new_set, org_set;
    sigemptyset (&new_set);
    sigaddset (&new_set, TIMER_SIG);
    pthread_sigmask(SIG_BLOCK, &new_set, &org_set);
*/
    smachine::timer::TimersPtr tmrs = std::make_shared<smachine::timer::Timers2>();
    if(tmrs){
        tmrs->put_event = std::bind(&handler, std::placeholders::_1);
        tmrs->init();

        std::cout << "Create timer" << std::endl;
        smachine::timer::timer_info tm_info = {100,0,5000000,true};
        tmrs->create_timer(tm_info);

        tm_info = {101,1,0,false};
        tmrs->create_timer(tm_info);

        tm_info = {102,0,9000000,true};
        tmrs->create_timer(tm_info);

        sleep(3);

        tmrs->reset_timer(101);
        sleep(6);

        tmrs->stop();

        std::cout << "100: " << tm_counters[0] << " 101: " << tm_counters[1] << " 102: " << tm_counters[2] << std::endl;
        std::cout << "Finished" << std::endl;
    }

    exit(EXIT_SUCCESS);
}