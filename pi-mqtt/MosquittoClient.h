/*
 * MosquittoClient.h
 *
 *  Created on: Apr 14, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQTT_MOSQUITTO_CL_H_
#define PI_MQTT_MOSQUITTO_CL_H_

#include <functional>
#include <mosquittopp.h>

#include "MqttDefines.h"
#include "MqttClientItf.h"

namespace mqtt {

class MosquittoClient : public mosqpp::mosquittopp, public MqttClientItf
{
public:       
    MosquittoClient(const char* clientID);
    virtual ~MosquittoClient();

    virtual const int cl_connect(const MqttServerInfo& conf);
    virtual const int cl_disconnect();
    virtual const std::string cl_get_version() const;
    virtual const int cl_publish(int* mid, const std::string& topic, const std::string& payload) override;
    virtual const int cl_publish(int* mid, const std::string& topic, const int payloadsize, const void* payload) override;
    

    virtual void on_connect(int rc) override;;
    virtual void on_disconnect(int rc) override;;
    virtual void on_publish(int mid) override;;
    virtual void on_message(const struct mosquitto_message * message) override {return;}
    virtual void on_subscribe(int mid, int qos_count, const int * granted_qos) override;;
    virtual void on_unsubscribe(int mid) override;;
    virtual void on_log(int level, const char * str) override;;
    virtual void on_error() override;;    

    unsigned int reconnect_delay = 2;
    unsigned int reconnect_delay_max = 6;

    const std::string cl_get_errname(const int code) const{
        switch(code){
            case MOSQ_ERR_CONN_PENDING: return "Connection pending";
            case MOSQ_ERR_SUCCESS: return "Success";
            case MOSQ_ERR_NOMEM: return "No memory";
            case MOSQ_ERR_PROTOCOL: return "Protocol";
            case MOSQ_ERR_INVAL: return "Invalid value";
            case MOSQ_ERR_NO_CONN: return "No Connection";
            case MOSQ_ERR_CONN_REFUSED: return "Connection refused";
            case MOSQ_ERR_NOT_FOUND: return "Not found";
            case MOSQ_ERR_CONN_LOST: return "Connection lost (7)";
            case MOSQ_ERR_TLS: return "TLS (8)";
            case MOSQ_ERR_PAYLOAD_SIZE: return "Payload size (9)";
            case MOSQ_ERR_NOT_SUPPORTED: return "Not supported (10)";
            case MOSQ_ERR_AUTH: return "Authentication (11)";
            case MOSQ_ERR_ACL_DENIED: return "ACL denied (12)";
            case MOSQ_ERR_UNKNOWN: return "Unknown (13)";
            case MOSQ_ERR_ERRNO: return "Err NO (14)";
            case MOSQ_ERR_EAI: return "EAI (15)";
            case MOSQ_ERR_PROXY: return "Proxy (16)";
            default: return std::to_string(code);
        }
    }

virtual bool const is_connected() const override { return m_connected; }	

private:

    void set_connected(const bool connected){
        m_connected = connected;
    }

    int m_qos;
    bool m_connected;
};

} /*end namespace mqtt*/

#endif