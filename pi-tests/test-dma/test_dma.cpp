#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"

#include "PiRobot.h"
#include "core_memory.h"
#include "core_pwm.h"

using namespace std;


/*
* Simple DMA test.
* Copy memory buffer to another one using DMA 10
*
*/
int main (int argc, char* argv[])
{
    size_t buff_size_bytes = 1024*10;
    bool success = true;

    pirobot::PiRobot* rbt = nullptr;
    pi_core::core_pwm::PwmCore* pwm = nullptr;
    pi_core::core_mem::PhysMemory* pmem = nullptr;
    std::shared_ptr<pi_core::core_mem::MemInfo> minfo;
    uintptr_t src = 0L;

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

    pwm = new pi_core::core_pwm::PwmCore();
    success = pwm->Initialize();

    if( !success ){
      std::cout << "Could not initialize PWM " << std::endl;
      goto clear_data;
    }


    pmem = new pi_core::core_mem::PhysMemory();

    minfo = pmem->get_memory(buff_size_bytes);
    if( minfo ){
        std::cout << "Allocated " << std::dec << minfo->get_size() << " bites. VAddr: " << std::hex
                            << minfo->get_vaddr() << " PhysAddr: " << std::hex << minfo->get_paddr() << std::endl;
    }
    else{
        std::cout << "Failed to allocate: " << std::dec << buff_size_bytes << " bytes" << std::endl;
        success = false;
    }


    src = (0x40000000 | minfo->get_paddr());
    std::cout << "Start to process DMA Control Block SRC: " << std::hex << src << std::endl;
    success = pwm->write_data(src, minfo->get_size());

    std::cout << "Write data returned " << success << std::endl;

  clear_data:

    if(pwm != nullptr){
      delete pwm;
    }

    if(rbt != nullptr){
      delete rbt;
    }

    if(pmem != nullptr){
      pmem->free_memory(minfo);
      delete pmem;
    }

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
