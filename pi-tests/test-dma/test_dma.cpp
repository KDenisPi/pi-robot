#include <stdio.h>
#include <stdlib.h>

#include "core_dma.h"

/*
* Simple DMA test.
* Copy memory buffer to another one using DMA 10
*
*/
int main (int argc, char* argv[])
{
    size_t buff_size = 1024;
    size_t buff_size_bytes = sizeof(uint32_t) * buff_size;
    uint32_t* src = new uint32_t(buff_size);
    uint32_t* dst = new uint32_t(buff_size);

    std::cout << "Starting..." << std::endl;


    memset((void*)src, 0, buff_size_bytes);
    memset((void*)src, 0, buff_size_bytes);

    pi_core::core_dma::DmaControlBlock* cb = new pi_core::core_dma::DmaControlBlock();
    cb->Initialize(reinterpret_cast<std::uintptr_t>(src), reinterpret_cast<std::uintptr_t>(dst), buff_size_bytes);

    std::cout << "DMA Control Block Initialized" << std::endl;

    pi_core::core_dma::DmaControl* dma = new pi_core::core_dma::DmaControl();
    dma->Initialize();
    std::cout << "DMA Initialized" << std::endl;

    dma->process_control_block(reinterpret_cast<std::uintptr_t>(cb));
    std::cout << "Start to process DMA Control Block" << std::endl;

    for(;;){
        if(dma->is_finished()){
            break;
        }
    }

    std::cout << "Finished" << std::endl;
    exit(EXIT_SUCCESS);
}