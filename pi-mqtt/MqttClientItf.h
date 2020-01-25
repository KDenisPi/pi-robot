/*
 * MqttClientItf.h
 *
 *  Created on: Jul 3, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQTT_CLIENTITF_H_
#define PI_MQTT_CLIENTITF_H_

#include "MqttDefines.h"
#include "MqttServerInfo.h"

namespace mqtt {

/*
* Interface for client implementation
*/
class MqttClientItf {
public:
    MqttClientItf() :
        owner_notification(nullptr),
        m_err_connect(0),
        err_conn_max(3) {}

    virtual ~MqttClientItf() {}

    virtual const int cl_connect(const MqttServerInfo& conf) = 0;
    virtual const int cl_disconnect() = 0;
    virtual const std::string cl_get_version() const = 0;
    virtual const int cl_publish(const std::string& topic, const std::string& payload) = 0;
    virtual const int cl_subscribe(const std::string& topic) {return mqtt::MQTT_ERROR_SUCCESS;}

    virtual void cl_notify(mqtt::MQTT_CLIENT_STATE state, mqtt::MQTT_CLIENT_ERROR code) const {
        if(owner_notification != nullptr){
            owner_notification(state, code);
        }
    }

    std::function<void(MQTT_CLIENT_STATE state, MQTT_CLIENT_ERROR code)> owner_notification;

    const bool is_max_err_conn() const {
        return (m_err_connect >= err_conn_max);
    }

    void err_conn_inc() {
        m_err_connect++;
    }

    virtual bool const is_connected() const = 0;

private:
    int m_err_connect; //connection error counter
    int err_conn_max;
};

} //end namespace mqtt

#endif