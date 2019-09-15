#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "core_mailbox.h"

using namespace std;


/*
* Mailbox access test.
*
*/
int main (int argc, char* argv[])
{
    bool success = true;

    size_t size_1 = 1024*100;
    std::shared_ptr<pi_core::MemInfo> minfo;
    std::string msg("There is the string");

    std::cout << "Starting..." << std::endl;

    pi_core::core_mailbox::MailboxCore* pmem = new pi_core::core_mailbox::MailboxCore();

    std::cout << "Allocate: " << size_1 << " bytes" << std::endl;
    minfo = pmem->get_memory(size_1);
    if( minfo ){
        std::cout << "Allocated " << std::dec << minfo->get_size() << " bites. VAddr: " << std::hex << minfo->get_vaddr() << " PAddr 0x" << std::hex << minfo->get_paddr() << std::endl;

        char* vp = (char*)minfo->get_vaddr();
        std::cout << "Copy string to buffer: " <<  msg.length() << " bytes" << std::endl;
        std::strcpy(vp, msg.c_str());

        printf("String: %s\n", vp);
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
