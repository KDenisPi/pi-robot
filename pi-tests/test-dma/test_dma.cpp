#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"

#include "PiRobot.h"
#include "core_memory.h"
#include "core_pwm.h"

#include "sled_data.h"

using namespace std;


/*
* Simple DMA test.
* Copy memory buffer to another one using DMA 10
*
*/

#define LED_COUNT 10

int main (int argc, char* argv[])
{
    size_t buff_size_bytes = LED_COUNT * 3 * 3 + 15; //10 leds
    std::uint32_t ldata[LED_COUNT] = {0x00AA0000, 0x00AA0000, 0x00AA0000, 0x00AA0000, 0x00AA0000, 0x00AA0000, 0x00AA0000, 0x00AA0000, 0x00AA0000, 0x00AA0000};
    bool success = true;

    pirobot::PiRobot* rbt = nullptr;
    pi_core::core_pwm::PwmCore* pwm = nullptr;
    std::shared_ptr<pi_core::core_mem::MemInfo> minfo;

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

    minfo = pwm->get_memory(buff_size_bytes);
    if( minfo ){
        std::cout << "Allocated " << std::dec << minfo->get_size() << " bites. VAddr: " << std::hex
                            << minfo->get_vaddr() << " PhysAddr: " << std::hex << minfo->get_paddr() << std::endl;

        std::uint8_t rgb[3];
        volatile std::uint8_t* dbuff = static_cast<volatile std::uint8_t*>(minfo->get_vaddr());
        for( std::size_t lidx = 0; lidx < LED_COUNT; lidx++ ){

          // Convert 0RGB to R,G,B
          rgb[0] = (ldata[lidx] & 0xFF);
          rgb[1] = ((ldata[lidx] >> 8 ) & 0xFF);
          rgb[2] = ((ldata[lidx] >> 16 ) & 0xFF);

          //
          // Replace each R,G,B byte by 3 bytes
          //
          for(int rgb_idx = 0; rgb_idx < 3; rgb_idx++){
              int idx = (lidx + rgb_idx)*3;
              dbuff[idx] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]];
              dbuff[idx+1] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]+1];
              dbuff[idx+2] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]+2];
          }
        }

    }
    else{
        std::cout << "Failed to allocate: " << std::dec << buff_size_bytes << " bytes" << std::endl;
        success = false;
    }

    std::cout << "Start to process DMA Control Block SRC: " << std::hex << minfo->get_paddr() << std::endl;
    success = pwm->write_data(minfo->get_paddr(), minfo->get_size());

    std::cout << "Write data returned " << success << std::endl;

  clear_data:

    if(pwm != nullptr){
      std::cout << "Free memory for buffer" << std::endl;
      pwm->free_memory(minfo);

      std::cout << "Delete PWM object" << std::endl;
      delete pwm;
    }

    if(rbt != nullptr){
      delete rbt;
    }

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
