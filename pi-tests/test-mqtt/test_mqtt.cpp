#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "pi-main.h"

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

    std::unique_ptr<pimain::PiMain> pm = std::unique_ptr<pimain::PiMain>(new pimain::PiMain());

    pm->set_conf_main("./test-mqtt.json");
    pm->set_debug_mode(true);
    pm->run();

    std::allocator<char> alloc;
    std::shared_ptr<char> mem = std::allocate_shared<char>(alloc,50);

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
