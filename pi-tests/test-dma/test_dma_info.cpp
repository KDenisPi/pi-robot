#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"

#include "core_dma.h"

using namespace std;


/*
* Simple DMA test.
* Copy memory buffer to another one using DMA 10
*
*/

int main (int argc, char* argv[])
{
  bool success = true;

  pi_core::core_dma::DmaInfo* dinfo = new pi_core::core_dma::DmaInfo();

  for(int i = 0; i < 5; i++){
    dinfo->dma_channel_check(i);
  }

  delete dinfo;

  std::cout << "Finished " << success << std::endl;
  exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
