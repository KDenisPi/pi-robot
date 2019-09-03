#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "core_pwm.h"
#include "core_mailbox.h"
#include "sled_data.h"
#include "PiRobot.h"

using namespace std;


/*
* Simple PWM & PWM Clock test.
* Start PWM object and configura PWM Clock
*
*/
int main (int argc, char* argv[])
{
    bool success = true;
    int lcount = 10;
    size_t buff_size_bytes = lcount * 3 * 3 + 15; //10 leds
    uint32_t ldata = 0x00505050;

    std::shared_ptr<pi_core::MemInfo> m_src;
    pirobot::PiRobot* rbt = nullptr;
    pi_core::core_pwm::PwmCore* pwm = nullptr;

    std::cout << "Starting..." << std::endl;
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


    pwm = new  pi_core::core_pwm::PwmCore();
    success = pwm->Initialize();
    if(success){
      sleep(1);

      pi_core::core_mailbox::MailboxCore* mmem = new pi_core::core_mailbox::MailboxCore();
      m_src = mmem->get_memory(buff_size_bytes);
      if( m_src ){
        std::cout << "Allocated " << std::dec << m_src->get_size() << " bites. VAddr: " << std::hex  << m_src->get_vaddr() << " PhysAddr: " << std::hex << m_src->get_paddr() << std::endl;
      }
      else{
        std::cout << "Failed to allocate: " << std::dec << buff_size_bytes << " bytes" << std::endl;
        success = false;
      }

      if(success){
        std::uint8_t* p_src = (std::uint8_t*)m_src->get_vaddr();
        volatile uintptr_t py_src = m_src->get_paddr();

        for( std::size_t lidx = 0; lidx < lcount; lidx++ ){

               // Convert 0RGB to R,G,B
                std::uint8_t rgb[3] = {
                    (uint8_t)(ldata & 0xFF),
                    (uint8_t)((ldata >> 8) & 0xFF),
                    (uint8_t)((ldata >> 16 ) & 0xFF)
                };

                    //
                    // Replace each R,G,B byte by 3 bytes
                    //
                    for(int rgb_idx = 0; rgb_idx < 3; rgb_idx++){
                        int idx = (lidx + rgb_idx)*3;
                        p_src[idx] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]];
                        p_src[idx+1] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]+1];
                        p_src[idx+2] |= pirobot::sledctrl::sled_data[rgb[rgb_idx]+2];
                    }
        }

        success = pwm->write_data(py_src, buff_size_bytes);
        std::cout << "PWM write_data finished: " << success << std::endl;
      }

      mmem->free_memory(m_src);
      delete mmem;
    }

clear_data:
    std::cout << "Release objects" << std::endl;

    delete pwm;
    delete rbt;

    std::cout << "Finished " << success << std::endl;

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
