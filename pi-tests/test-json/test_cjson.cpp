#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>

#include "cJSON.h"

#include "file_loader.h"

using namespace std;

int main (int argc, char* argv[])
{
    if(argc==1){
        std::cout << "No configuration file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::unique_ptr<piutils::floader::Floader> fl = std::unique_ptr<piutils::floader::Floader>(new piutils::floader::Floader(argv[1]));
    if(fl->is_success()){
        std::cout << "Loaded file " << fl->get_name() << " Size: " << fl->get_size() << std::endl;
        std::shared_ptr<cJSON> cjson = std::shared_ptr<cJSON>(cJSON_Parse(fl->get()));
    }
    else{
        std::cout << "Could not load file " << fl->get_name() << " Error: " << fl->get_err() << std::endl;
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
