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

namespace mqqt {

//
//
//
class MqqtObject {
public:
    MqqtObject(const std::string& topic, const std::string& payload){
        m_payloadsize = payload.length();
        m_topic = std::move(topic);
        m_payload = std::shared_ptr<const void>(std::move((const void*)payload.c_str()));
    }

    MqqtObject(const std::string& topic, const int payloadsize, const void* payload){
        m_payloadsize = payloadsize;
        m_topic = std::move(topic);
        m_payload = std::shared_ptr<const void>(std::move(payload));
    }

    MqqtObject() : m_payloadsize(0) {

    }

    virtual ~MqqtObject(){
        m_payloadsize = 0;
        m_topic.clear();
    }
/*
    MqqtObject& operator=(const MqqtObject& node){
        m_topic = node.topic();
        m_payload = node.payload();
        m_payloadsize = node.payloadsize();
    }
*/
    MqqtObject& operator=(MqqtObject&& node){
        m_topic = std::move(node.topic());
        m_payload = std::move(node.payload());
        m_payloadsize = node.payloadsize();
    }
    //
    //
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
    const std::shared_ptr<const void> payload() const{
        return m_payload;
    }

    const int payloadsize() const {
        return m_payloadsize;
    }

private:
    std::string m_topic;
    std::shared_ptr<const void> m_payload;
    int m_payloadsize;
};

} //namespace mqqt
#endif