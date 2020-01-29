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
    mqtt::MqttServerInfo mqtt_conf("127.0.0.1", "sensor", true);
    std::shared_ptr<mqtt::MqttItf> mqtt = std::make_shared<mqtt::MqttClient<mqtt::MosquittoClient>>(mqtt_conf);
    bool mqtt_active = mqtt->start();

    sleep(1);
    mqtt->subscribe("sensor");
    mqtt->subscribe("sensor1");
    mqtt->publish("sensor", "Message1");
    mqtt->publish("sensor1", "Message11");
    mqtt->publish("sensor", "Message111");

    sleep(5);
    mqtt->unsubscribe("sensor");
    mqtt->unsubscribe("sensor1");

    sleep(1);
    mqtt->stop();
    mqtt.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
