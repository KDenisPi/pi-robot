/*
 * MqttServerInfo.h
 *
 *  Created on: Jan 17, 2018
 *      Author: Denis Kudia
 */
#ifndef PI_MQTT_SERVER_INFO_H_
#define PI_MQTT_SERVER_INFO_H_

#include "MqttDefines.h"

namespace mqtt {

/*
* MQTT server configuration information
*/
class MqttServerInfo {
public:
    /*
    *
    */
    MqttServerInfo(const std::string& host = "", const std::string& clientid = "") :
        _mqtt_enable(false), m_host(host), m_clientid(clientid), m_port(1883), m_keepalive(10), m_qos(0),
        m_tls(false), m_tls_insecure(false), m_tls_version("tlsv1.2") {

    }

    virtual ~MqttServerInfo() {}

    const char* host() const {return m_host.c_str();}
    void set_host(const std::string& host){
        m_host = host;
    }

    const char* clientid() const {return (m_clientid.empty() ? NULL : m_clientid.c_str());}
    void set_clientid(const std::string& clientid){
        m_clientid = clientid;
    }

    void set_port(const int port) {m_port = port;}
    const int port() const { return m_port;}

    void set_keepalive(const int keepalive){ m_keepalive = keepalive;}
    const int keepalive()const {return m_keepalive;}

    const int qos() const { return m_qos;}
    void set_qos(const int qos) {
        if(qos>=0 && qos<=2){
            m_qos = qos;
        }
    }

    const bool is_enable() const {
        return _mqtt_enable;
    }

    void set_enable(const bool mqtt_enable){
        _mqtt_enable = mqtt_enable;
    }

    /*
    TLS section
    */
    const bool is_tls() const{
        return m_tls;
    }
    void set_tls(const bool tls){
        m_tls = tls;
    }

    // TLS insecure
    const bool is_tls_insecure() const{
        return m_tls_insecure;
    }
    void set_tls_insecure(const bool tls_insecure){
        m_tls_insecure = tls_insecure;
    }

    // CA file
    const std::string get_cafile() const{
        return m_cafile;
    }
    void set_cafile(const std::string& cafile){
        m_cafile = cafile;
    }

    // TLS version
    const std::string get_tls_version() const{
        return m_tls_version;
    }
    void set_tls_version(const std::string& tls_version){
        m_tls_version = tls_version;
    }

    /*
    *
    */
    MqttServerInfo& operator=(const MqttServerInfo& info){
        if(this != &info){
            this->_mqtt_enable = info._mqtt_enable;
            this->m_host = info.m_host;
            this->m_clientid = info.m_clientid;
            this->m_port = info.m_port;
            this->m_keepalive = info.m_keepalive;
            this->m_tls = info.m_tls;
            this->m_cafile = info.m_cafile;
            this->m_tls_insecure = info.m_tls_insecure;
            this->m_tls_version = info.m_tls_version;
        }
        return *this;
    }

    /*
    *
    */
    const std::string to_string(){
        return std::string("Host: [") + m_host + "] id: [" + m_clientid  +"] port: " +
            std::to_string(m_port) + " keep alive: " + std::to_string(m_keepalive) +
            " TLS:" + std::to_string(m_tls) + " TLS version: " + m_tls_version + " Insecure:" + std::to_string(m_tls_insecure);
    }

private:
    bool _mqtt_enable;          //MQTT enabled
    std::string m_clientid;     //Client machine name
    std::string m_host;         //Host name. Note: For TLS we should use the same host name as on server certificate (ignored if m_tls_insecure=true)
    int m_port;                 //Port value 1883 (8883 for TLS On)
    int m_keepalive;
    int m_qos;

    bool m_tls;                 //Use TLS
    std::string m_cafile;       //CA file
    bool m_tls_insecure;        //Insecure flag for TLS (do not check host name in server certificate)
    std::string m_tls_version;  //TLS version. Possible values [tlsv1.2, tlsv1.1, tlsv1]
};


}

#endif