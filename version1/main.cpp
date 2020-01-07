#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>

#include "MqttServerInfo.h"

using namespace std;

int main (int argc, char* argv[])
{
    cout <<  "Raspberry Pi blink Parameters:" << argc << endl;
 
    std::string filename = "/home/denis/pi-robot/project1/project1.json";
    std::string mqtt_filename = "/home/denis/pi-robot/pi-mqtt/pi-mqtt-conf.json";

    mqtt::MqttServerInfo info = mqtt::MqttServerInfo::load(mqtt_filename);

    exit(0);
}
