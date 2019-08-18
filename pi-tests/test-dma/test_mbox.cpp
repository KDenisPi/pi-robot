#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "core_memory.h"

using namespace std;


/*
* Mailbox access test.
*
*/
int main (int argc, char* argv[])
{
    bool success = true;

    size_t size_1 = 1024*100;
    std::shared_ptr<pi_core::core_mem::MemInfo> minfo;

    std::cout << "Starting..." << std::endl;

    pi_core::core_mem::PhysMemory* pmem = new pi_core::core_mem::PhysMemory();

    std::cout << "Allocate: " << size_1 << " bytes" << std::endl;
    minfo = pmem->get_memory(size_1, 3);
    if( minfo ){
        std::cout << "Allocated " << std::dec << minfo->get_size() << " bites. VAddr: " << std::hex << minfo->get_vaddr() << " PAddr " << std::hex << minfo->get_paddr() << std::endl;
    }
    else{
        std::cout << "Failed to allocate: " << size_1 << " bytes" << std::endl;
        success = false;
    }
    pmem->free_memory(minfo);

    delete pmem;

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));

}
