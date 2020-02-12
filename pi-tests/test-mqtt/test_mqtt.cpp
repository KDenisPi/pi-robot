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

    std::string msg_def = "{ \"Temperature\": 23.5, \"Humidity\": 65.5, \"Luximity\": 13, \"TVOC\": 24, \"CO2\": 440, \"Pressure\": 672 }";
    std::string host = (argc > 1 ? argv[1] : "127.0.0.1");
    std::string topic = (argc > 2 ? argv[2] : "pirobot/sensors");
    std::string msg = (argc > 3 ? argv[3] : msg_def);

    std::cout << "Host: " << host << " Topic: " << topic << "Message [" << msg << "]" << std::endl;

    mqtt::MqttServerInfo mqtt_conf(host, topic, true);
    std::shared_ptr<mqtt::MqttItf> mqtt = std::make_shared<mqtt::MqttClient<mqtt::MosquittoClient>>(mqtt_conf);
    bool mqtt_active = mqtt->start();

    sleep(1);
    mqtt->subscribe(topic);
    mqtt->publish(topic, msg);
    mqtt->publish(topic, msg + " 001");
    mqtt->publish(topic, msg + " 002");

    sleep(5);
    mqtt->unsubscribe(topic);

    sleep(1);
    mqtt->stop();
    mqtt.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
