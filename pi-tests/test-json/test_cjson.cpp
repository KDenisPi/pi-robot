#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>

#include "cJSON_wrap.h"

using namespace std;

int main (int argc, char* argv[])
{
    if(argc==1){
        std::cout << "No configuration file" << std::endl;
        exit(EXIT_FAILURE);
    }

    const std::unique_ptr<piutils::floader::Floader> fl = std::unique_ptr<piutils::floader::Floader>(new piutils::floader::Floader(argv[1]));
    if(!fl->is_success()){
        std::cout << "Could not load file " << fl->get_name() << " Error: " << fl->get_err() << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Loaded file " << fl->get_name() << " Size: " << fl->get_size() << std::endl;
    std::shared_ptr<piutils::cjson_wrap::CJsonWrap> cjson = std::make_shared<piutils::cjson_wrap::CJsonWrap>(fl);

    try{
        std::string ver = cjson->get_attr_string(cjson->get(), std::string("version"));
        std::cout << "Version: " << ver << std::endl;
        bool real_w = cjson->get_attr<bool>(cjson->get(), std::string("real_world"));
        std::cout << "real_world: " << real_w << std::endl;
        int32_t real_wi = cjson->get_attr<int32_t>(cjson->get(), std::string("real_world"));
    }
    catch(std::runtime_error& exc){
        std::cout << "Error: "<< + exc.what() << std::endl;
    }

    exit(EXIT_SUCCESS);
}
