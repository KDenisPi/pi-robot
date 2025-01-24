#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "Mqtt_object.h"


using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;
    std::shared_ptr<piutils::circbuff::CircularBuffer<std::shared_ptr<mqtt::MqttObject>>> m_messages;



    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
