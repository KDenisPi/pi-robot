#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>

#include "MqqtServerInfo.h"

using namespace std;

int main (int argc, char* argv[])
{
    cout <<  "Raspberry Pi blink Parameters:" << argc << endl;
 
    std::string filename = "/home/denis/pi-robot/project1/project1.json";
    std::string mqqt_filename = "/home/denis/pi-robot/pi-mqqt/pi-mqqt-conf.json";

    mqqt::MqqtServerInfo info = mqqt::MqqtServerInfo::load(mqqt_filename);

    exit(0);
}
