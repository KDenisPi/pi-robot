#include <stdio.h>
#include <stdlib.h>
#include <memory>

//#include "pi-main.h"
#include "MqttClient.h"
#include "MosquittoClient.h"
#include "MqttServerInfo.h"

using namespace std;


/*
* Simple MQTT test.
* Create MQTT client, connect to the server, publish information
*
*/

int main (int argc, char* argv[])
{
    bool success = true;
    std::cout << "Starting..." << std::endl;

/*
    std::unique_ptr<pimain::PiMain> pm = std::unique_ptr<pimain::PiMain>(new pimain::PiMain());
    pm->set_conf_main("./test-mqtt.json");
    pm->set_debug_mode(true);
    pm->run();
*/
    mqtt::MqttServerInfo mqtt_conf("10.0.1.11", "sensor", true);
    std::shared_ptr<mqtt::MqttItf> mqtt = std::make_shared<mqtt::MqttClient<mqtt::MosquittoClient>>(mqtt_conf);
    bool mqtt_active = mqtt->start();



    mqtt->stop();
    mqtt.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
