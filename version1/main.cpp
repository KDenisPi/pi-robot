#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>

#include "jsoncons/json.hpp"

using namespace std;

template<class T>
T get_attr(const jsoncons::json& object, const std::string& name, const T& attr_default){
    return (object.has_key(name) ? object[name].as<T>() : attr_default);
};

int main (int argc, char* argv[])
{
    cout <<  "Raspberry Pi blink Parameters:" << argc << endl;
 
    std::string filename = "/home/denis/pi-robot/project1/project1.json";

    std::ifstream ijson(filename);
    try{
        cout <<  "Parse JSON " << filename << endl;
        jsoncons::json conf = jsoncons::json::parse(ijson);

        auto providers  =  conf["providers"];
        for(const auto& provider : providers.array_range()){
            std::string provider_type = provider["type"].as<std::string>();
            std::string provider_name = (provider.has_key("name") ? provider["name"].as<std::string>() : provider_type);

            auto pins = get_attr<int>(provider, "pins", 11);
            cout << provider_type << " " << provider_name << " Pins:" << pins <<endl;
        }
    
    }
    catch(jsoncons::parse_error& perr){
         cout << perr.what() << " Line: " <<  perr.line_number() << " Column: " << perr.column_number() << endl;
    }
 
  //mqqt::MqqtServerInfo info(mqqt::MqqtServerInfo("10.0.0.9", "pi-robot"));
 //mqqt::MqqtClient<mqqt::MosquittoClient>* clMqqt = new mqqt::MqqtClient<mqqt::MosquittoClient>(info);
 //clMqqt->connect();
 //clMqqt->disconnect();
 //delete clMqqt;


    exit(0);
}
