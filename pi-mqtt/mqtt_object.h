/*
 * mqtt_object.h
 *
 *  Created on: Jan 26, 2018
 *      Author: Denis Kudia
 */


#ifndef MQTT_OBJECT_H
#define MQTT_OBJECT_H

#include <string>
#include <memory>

#include "logger.h"

namespace mqtt {

const char TAG_MQ[] = "mqttobj";

//
//
//
class MqttObject {
public:
    MqttObject(const std::string& topic, const std::string& payload) {
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " started");

        _payloadsize = payload.length();
        _topic = std::move(topic);
        _payload = std::move(payload);

        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " finished");
    }

    virtual ~MqttObject(){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Destructor ");
        clear();
    }

    MqttObject& operator=(const MqttObject& node){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Copy 1");

        _topic = node.topic();
        _payloadsize = node.payloadsize();
        _payload = node.payload();

        return *this;
    }

    MqttObject& operator=(MqttObject&& node){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Copy 2");

        _topic = std::move(node.topic());
        _payloadsize = node.payloadsize();
        _payload = std::move(node.payload());

        return *this;
    }

    //
    const bool empty()  const{
        return (_payloadsize==0);
    }

    //
    //
    //
    const std::string& topic() const{
        return _topic;
    }

    //
    //
    //
    const std::string& payload() const{
        return _payload;
    }

    const int payloadsize() const {
        return _payloadsize;
    }

    const std::string to_string() const {
        std::string msg = " Topic: [" + _topic + "] Payload: [" + (_payload.length()>0 ? "Data" : "Empty") + "] Size: " + std::to_string(_payloadsize);
        return msg;
    }

private:
    std::string _topic;
    std::string _payload;
    int _payloadsize;

    void clear() {
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Clear ");

        _payload.clear();
        _payloadsize = 0;
        _topic.clear();
    }
};

} //namespace mqtt
#endif