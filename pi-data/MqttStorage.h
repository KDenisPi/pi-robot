/*
 * MqttStorage.h
 *
 *  Mqtt based data storage results
 *
 *  Created on: Feb 14, 2020
 *      Author: Denis Kudia
 */

#ifndef PIDATA_MQTT_STORAGE_H_
#define PIDATA_MQTT_STORAGE_H_

#include "DataStorage.h"
#include "MqttClient.h"
#include "MosquittoClient.h"
#include "MqttServerInfo.h"

namespace pidata{
namespace mqttstorage {

/*
* Implementation of MQTT protocol based data storage
*/
template<typename T>
class MqttStorage : public pidata::datastorage::DataStorage<T> {
public:
    /*
    *
    */
    MqttStorage() {
    }

    /*
    *
    */
    virtual ~MqttStorage() {
    }

    /*
    *
    */
    bool start(const mqtt::MqttServerInfo mqtt_conf) {

        if(!mqtt_conf.is_enable()){
            logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "MQTT Disabled");
            return false;
        }

        m_mqtt = std::make_shared<mqtt::MqttClient<mqtt::MosquittoClient>>(mqtt_conf);
        datastorage::DataStorage<T>::set_run(m_mqtt->start());

        logger::log(logger::LLOG::INFO, "main", std::string(__func__) + "MQTT configuration loaded Active: " + std::to_string(datastorage::DataStorage<T>::is_run()));
        return  datastorage::DataStorage<T>::is_run();
    }

    /*
    *
    */
    virtual void stop() override{
        m_mqtt->stop();
    }

    virtual const uint32_t get_status() override {
        uint32_t status = pidata::datastorage::s_DOWN;

        if(datastorage::DataStorage<T>::is_run()) status |= pidata::datastorage::s_UP;
        if(m_mqtt->is_connected()) status |= pidata::datastorage::s_CONNECTED;

        return status;
    }

    void set_topic(const std::string& topic){
        _topic = topic;
    }

    const std::string topic() const {
        return _topic;
    }

protected:
    virtual const mqtt::MQTT_CLIENT_ERROR mqtt_publish(const std::string& topic, const std::string& payload) {
        if(datastorage::DataStorage<T>::is_run())
            return m_mqtt->publish(topic, payload);

        return mqtt::MQTT_CLIENT_ERROR::MQTT_ERROR_SUCCESS;
    }

private:
    std::shared_ptr<mqtt::MqttItf> m_mqtt;
    std::string _topic;

};


}//namespace mqttstorage
}//namespace pidata

#endif
