#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"

#include "core_dma.h"
#include "core_pwm.h"
#include "PiRobot.h"

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
    char* src = nullptr;

    pirobot::PiRobot* rbt = nullptr;
    pi_core::core_dma::DmaControlBlock* cb = nullptr;
    pi_core::core_pwm::PwmCore* pwm = nullptr;
    pi_core::core_clock_pwm::PwmClock* clk = new pi_core::core_clock_pwm::PwmClock();

    std::cout << "Starting..." << std::endl;

    success = clk->Initialize();

    delete clk;

  /*
    logger::log(logger::LLOG::DEBUG, "main", std::string(__func__) + " DMA test");

    rbt = new pirobot::PiRobot();
    if( !rbt->configure(argv[1])){
      std::cout << "Invalid configuration file " << std::string(argv[1]) << std::endl;
      success = false;
      goto clear_data;
    }

    rbt->printConfig();

    if( !rbt->start()){
      std::cout << "Could not start Pi-Robot " << std::endl;
      success = false;
      goto clear_data;
    }

    src = static_cast<char*>(malloc(buff_size_bytes));

    memset((void*)src, 'D', buff_size_bytes);
    src[buff_size_bytes-1] = 0x00;

    pwm = new pi_core::core_pwm::PwmCore();
    if( !pwm->Initialize()){
      std::cout << "Could not initialize PWM " << std::endl;
      success = false;
      goto clear_data;
    }

    cb = new pi_core::core_dma::DmaControlBlock();
    cb->Initialize(reinterpret_cast<std::uintptr_t>(src), pwm->get_fifo_address(), buff_size_bytes);

    std::cout << "TI register" << std::endl << pwm->ti_register_status() << std::endl;
    std::cout << "CS register" << std::endl << pwm->cs_register_status() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(5));
*/

/*
    std::cout << "Start to process DMA Control Block" << std::endl;
    if( !dma->process_control_block(reinterpret_cast<std::uintptr_t>(cb))){
        std::cout << "Could not start copy" << std::endl;
    }
    else {
      std::cout << "Waiting for finish processing DMA Control Block" << std::endl;
      for(int i = 0; i < 10; i++){
        if(dma->is_finished()){
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Waiting for finish processing DMA Control Block Error: " << dma->is_error() << " Paused: " << dma->is_paused() << " Wait: " << dma->is_waiting_for_outstanding_writes() << std::endl;
      }

      std::cout << "Finished processing DMA Control Block" << std::endl;
    }
*/

/*
  clear_data:

    if(pwm != nullptr){
      delete pwm;
    }

    if(cb != nullptr){
      delete cb;
    }

    if(src != nullptr){
      free(src);
    }

    delete rbt;
*/
    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
