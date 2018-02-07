/*
 * mqqt_object.h
 *
 *  Created on: Jan 26, 2018
 *      Author: Denis Kudia
 */


#ifndef MQQT_OBJECT_H
#define MQQT_OBJECT_H

#include <string>
#include <memory>

#include "logger.h"

namespace mqqt {

const char TAG_MQ[] = "mqqtobj";

//
//
//
class MqqtObject {
public:
    MqqtObject(const std::string& topic, const std::string& payload) { //: m_payload(nullptr) {

        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " [1] started");

        m_payloadsize = payload.length();
        m_topic = std::move(topic);
        m_payload = std::move(payload);

        //m_payload = new char[m_payloadsize];
        //std::memcpy(m_payload, payload.c_str(), m_payloadsize);

        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " [1] finished");
    }

    MqqtObject(const std::string& topic, const int payloadsize, const void* payload) { //: m_payload(nullptr){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " [2] " + topic);

        m_payloadsize = payloadsize;
        m_topic = std::move(topic);
        //m_payload = std::move(payload);

        //m_payload = new char[m_payloadsize];
        //std::memcpy(m_payload, payload, m_payloadsize);
    }

    MqqtObject() : m_payloadsize(0) { //, m_payload(nullptr) {
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " [3] ");

    }

    virtual ~MqqtObject(){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Destructor ");
        clear();
    }

    MqqtObject& operator=(const MqqtObject& node){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Copy 1");

        clear();
        m_topic = node.topic();
        m_payloadsize = node.payloadsize();
        m_payload = node.payload();

        //m_payload = new char[m_payloadsize];
        //std::memcpy(m_payload, node.payload(), m_payloadsize);

        return *this;
    }

    MqqtObject& operator=(MqqtObject&& node){
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Copy 2");

        m_topic = std::move(node.topic());
        m_payload = std::move(node.payload());
        m_payloadsize = node.payloadsize();
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
    const std::string& payload() const{
        return m_payload;
    }

    const int payloadsize() const {
        return m_payloadsize;
    }

    const std::string to_string() const {
        //std::string msg = " Topic: [" + m_topic + "] Payload: [" + (m_payload != nullptr ? "Data" : "Empty") + "] Size: " + std::to_string(m_payloadsize);
        std::string msg = " Topic: [" + m_topic + "] Payload: [" + m_payload + "] Size: " + std::to_string(m_payloadsize);
        return msg;
    }

private:
    std::string m_topic;
    //void* m_payload;
    std::string m_payload;
    int m_payloadsize;

    void clear() {
        logger::log(logger::LLOG::DEBUG, TAG_MQ, std::string(__func__) + " Clear ");
        /*
        if(m_payload != nullptr){
            delete[] (char*)m_payload;
            m_payload = nullptr;
        }
        */
        m_payload.clear();
        m_payloadsize = 0;
        m_topic.clear();
    }
};

} //namespace mqqt
#endif