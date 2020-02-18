#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "MqttStorage.h"
#include "MqttServerInfo.h"

using namespace std;


/*
* Simple MQTT test.
* Create MQTT client, connect to the server, publish information
*
*/

class MqttStorage : public pidata::mqttstorage::MqttStorage<std::string> {
public:

    MqttStorage(const std::string& topic) {
        set_topic(topic);
    }

    virtual ~MqttStorage() {
    }

    /*
    *
    */
    virtual bool write(const std::string& message) override {
        logger::log(logger::LLOG::DEBUG, "main", std::string(__func__) + "MQTT data: " + message);

        mqtt::MQTT_CLIENT_ERROR res = mqtt_publish(topic(), message);
        return (res == mqtt::MQTT_CLIENT_ERROR::MQTT_ERROR_SUCCESS);
    }
};

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
    std::shared_ptr<MqttStorage> mqttstorage = std::make_shared<MqttStorage>(topic);
    bool mqtt_active = mqttstorage->start(mqtt_conf);

    mqttstorage->write(msg);
    mqttstorage->write(msg + " 001");
    mqttstorage->write(msg + " 002");

    sleep(5);
    mqttstorage->stop();
    mqttstorage.reset();

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
