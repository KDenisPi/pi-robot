#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "core_dma.h"

/*
* Simple DMA test.
* Copy memory buffer to another one using DMA 10
*
*/
int main (int argc, char* argv[])
{
    size_t buff_size_bytes = 1024;
    std::cout << "Starting..." << std::endl;


    logger::log(logger::LLOG::DEBUG, "main", std::string(__func__) + " DMA test");

    char* src = static_cast<char*>(malloc(buff_size_bytes));
    char* dst = static_cast<char*>(malloc(buff_size_bytes));

    memset((void*)src, 'D', buff_size_bytes);
    src[buff_size_bytes-1] = 0x00;
    memset((void*)dst, 0, buff_size_bytes);


    pi_core::core_dma::DmaControlBlock* cb = new pi_core::core_dma::DmaControlBlock();
    cb->Initialize(reinterpret_cast<std::uintptr_t>(src), reinterpret_cast<std::uintptr_t>(dst), buff_size_bytes);

    std::cout << "DMA Control Block Initialized" << std::endl;

    pi_core::core_dma::DmaControl* dma = new pi_core::core_dma::DmaControl();

    dma->Initialize();
    std::cout << "DMA Initialized" << std::endl;

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

    delete dma;
    delete cb;
    free(src);
    free(dst);

    std::cout << "Finished" << std::endl;
    exit(EXIT_SUCCESS);
}
