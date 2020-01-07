#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"

using namespace std;


/*
* Simple DMA test.
* Copy memory buffer to another one using DMA 10
*
*/

int main (int argc, char* argv[])
{
    bool success = true;
    std::cout << "Starting..." << std::endl;

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
