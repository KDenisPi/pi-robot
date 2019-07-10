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

    pi_core::core_mailbox::MailboxCore* mbox = new pi_core::core_mailbox::MailboxCore();
    uint32_t firmvare = mbox->get_firmware_version();

    delete mbox;

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));

}