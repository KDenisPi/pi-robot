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
    MqttObject(const std::string& topic, const std::string& payload) : m_payload(nullptr) {

        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " [1] started");

        m_payloadsize = payload.length();
        m_topic = std::move(topic);

        char* p = new char[m_payloadsize];
        std::memcpy(p, payload.c_str(), m_payloadsize);
        m_payload = std::shared_ptr<char>(p);

        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " [1] finished");
    }

    MqttObject(const std::string& topic, const int payloadsize, const void* payload) : m_payload(nullptr){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " [2] " + topic);

        m_payloadsize = payloadsize;
        m_topic = std::move(topic);
        //m_payload = std::move(payload);
    }

    MqttObject() : m_payloadsize(0), m_payload(nullptr) {
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " [3] ");
    }

    virtual ~MqttObject(){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Destructor ");
        clear();
    }

    MqttObject& operator=(const MqttObject& node){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Copy 1");

        clear();
        m_topic = node.topic();
        m_payloadsize = node.payloadsize();

        char* p = new char[m_payloadsize];
        std::memcpy(p, node.payload().get(), m_payloadsize);
        m_payload = std::shared_ptr<char>(p);

        return *this;
    }

    MqttObject& operator=(MqttObject&& node){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Copy 2");

        m_topic = std::move(node.topic());
        m_payloadsize = node.payloadsize();
        m_payload = std::move(node.payload());

        return *this;
    }

    //
    const bool empty()  const{
        return (m_payloadsize == 0);
    }

    //
    //
    //
    const std::string& topic() const{
        return m_topic;
    }

    //
    //
    //
    const std::shared_ptr<char>& payload() const{
        return m_payload;
    }

    const int payloadsize() const {
        return m_payloadsize;
    }

    const std::string to_string() const {
        std::string msg = " Topic: [" + m_topic + "] Payload: [" + (m_payload ? "Data" : "Empty") + "] Size: " + std::to_string(m_payloadsize);
        return msg;
    }

private:
    std::string m_topic;
    std::shared_ptr<char> m_payload;
    int m_payloadsize;

    void clear() {
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Clear ");
        
        if(m_payload){
            delete[] (char*)m_payload.get();
            m_payload.reset();
        }
        m_payloadsize = 0;
        m_topic.clear();
    }
};

} //namespace mqtt
#endif