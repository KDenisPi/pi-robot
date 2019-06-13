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
    bool success = true;

    std::shared_ptr<pi_core::core_mem::MemInfo> m_src;
    std::shared_ptr<pi_core::core_mem::MemInfo> m_dst;

    pi_core::core_mem::PhysMemory* pmem = new pi_core::core_mem::PhysMemory();

    std::cout << "Starting..." << std::endl;

    m_src = pmem->get_memory(buff_size_bytes);
    m_dst = pmem->get_memory(buff_size_bytes);

    if( m_src ){
        std::cout << "Allocated " << std::dec << m_src->get_size() << " bites. VAddr: " << std::hex  << m_src->get_vaddr() << " PhysAddr: " << std::hex << m_src->get_paddr() << std::endl;
    }
    else{
        std::cout << "Failed to allocate: " << std::dec << buff_size_bytes << " bytes" << std::endl;
        success = false;
    }

    if( m_dst ){
        std::cout << "Allocated " << std::dec << m_dst->get_size() << " bites. VAddr: " << std::hex  << m_dst->get_vaddr() << " PhysAddr: " << std::hex << m_dst->get_paddr() << std::endl;
    }
    else{
        std::cout << "Failed to allocate: " << std::dec << buff_size_bytes << " bytes" << std::endl;
        success = false;
    }

    memset(m_src->get_vaddr(), 'A', buff_size_bytes - 1);
    memset(m_dst->get_vaddr(), 'B', buff_size_bytes - 1);

    if(success){

      pi_core::core_dma::DmaControl* dctrl = new pi_core::core_dma::DmaControl();
      pi_core::core_dma::DmaControlBlock* cb = new pi_core::core_dma::DmaControlBlock(pmem, pi_core::DREQ::NO_required);

      dctrl->Initialize(pi_core::core_dma::DmaControl::cs_flags_test);

      std::cout << "Start to process DMA Control Block SRC: " << std::hex << m_src->get_paddr() << " DST: " << m_dst->get_paddr() << std::endl;
      cb->prepare(m_src->get_paddr(), m_dst->get_paddr(), m_src->get_size());

      bool result = dctrl->process_control_block(cb);
      if( result ){
          int i = 0;
          while( !dctrl->is_finished() && ++i < 10){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            dctrl->print_status();
          }

          if(i>= 10){
             std::cout << " ******* FAILED ************" << std::endl;
             success = false;
          }
          else{
          }

          dctrl->print_status(true);
          dctrl->reset();
          dctrl->print_status(true);
     }

      //success = pwm->write_data(m_src->get_paddr(), minfo->get_size());

      delete cb;
      delete dctrl;
      std::cout << "Write data returned " << success << std::endl << std::endl ;
    }

  printf("SRC -> %s\n", (char*)m_src->get_vaddr() );
  printf("DST -> %s\n", (char*)m_dst->get_vaddr() );

  clear_data:

    std::cout << "****** Release memory " << success << std::endl << std::endl;

    pmem->free_memory(m_src);
    pmem->free_memory(m_dst);

    delete pmem;

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
