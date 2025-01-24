#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "fstorage.h"

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;
    piutils::fstor::FStorage fstor;

    fstor.collect_data_files("/var/data/pi-robot/data");

    for (std::string dfl : piutils::fstor::FStorage::dfiles) {
        std::cout << dfl << std::endl;
    }


    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
