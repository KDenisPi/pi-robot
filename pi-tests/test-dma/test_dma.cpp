#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"

#include "PiRobot.h"
#include "core_pwm.h"

using namespace std;


/*
* Simple DMA test.
* Copy memory buffer to another one using DMA 10
*
*/
int main (int argc, char* argv[])
{
    size_t buff_size_bytes = 1024;
    bool success = true;
    uint8_t* src = nullptr;

    pirobot::PiRobot* rbt = nullptr;
    pi_core::core_pwm::PwmCore* pwm = nullptr;

    std::cout << "Starting..." << std::endl;

    logger::log(logger::LLOG::DEBUG, "main", std::string(__func__) + " DMA test");
    if( argc < 2 ){
      std::cout << "Failed. No configuration file" << std::endl;
      exit(EXIT_FAILURE);
    }


    rbt = new pirobot::PiRobot();
    success = rbt->configure(argv[1]);
    if( !success ){
      std::cout << "Invalid configuration file " << std::string(argv[1]) << std::endl;
      goto clear_data;
    }

    rbt->printConfig();

    success = rbt->start();
    if( !success ){
      std::cout << "Could not start Pi-Robot " << std::endl;
      goto clear_data;
    }

    src = static_cast<uint8_t*>(malloc(buff_size_bytes));
    memset((void*)src, 'D', buff_size_bytes);
    src[buff_size_bytes-1] = 0x00;

    pwm = new pi_core::core_pwm::PwmCore();
    success = pwm->Initialize();

    if( !success ){
      std::cout << "Could not initialize PWM " << std::endl;
      goto clear_data;
    }

    std::cout << "Start to process DMA Control Block" << std::endl;
    success = pwm->write_data(src, buff_size_bytes);

    std::cout << "Write data returned " << success << std::endl;

  clear_data:

    if(pwm != nullptr){
      delete pwm;
    }

    if(src != nullptr){
      free(src);
    }

    if( rbt != nullptr ){
      delete rbt;
    }

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
