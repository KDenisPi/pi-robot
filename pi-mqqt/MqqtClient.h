/*
 * MqqtClient.h
 *
 *  Created on: Apr 17, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQQT_CLIENT_H_
#define PI_MQQT_CLIENT_H_

#include <memory>

#include "MqqtDefines.h"

namespace mqqt {

/*
* MQQT server configuration information
*/
class MqqtServerInfo {
public:    
    /*
    *
    */
    MqqtServerInfo(const std::string host) : 
        m_host(host), m_clientid(""), m_port(1883), m_keepalive(60), m_qos(0) {

    }

    /*
    *
    */
    MqqtServerInfo(const std::string host, const std::string clientid) : 
        m_host(host), m_clientid(clientid), m_port(1883), m_keepalive(60), m_qos(0) {

    }
    
    virtual ~MqqtServerInfo() {}

    bool is_empty() const {return m_host.empty();}
    const char* host() const {return m_host.c_str();}

    const char* clientid() const {return (m_clientid.empty() ? NULL : m_clientid.c_str());}
    
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

    /*
    *
    */
    MqqtServerInfo& operator=(const MqqtServerInfo& info){
        if(this != &info){
            this->m_host = info.m_host;
            this->m_clientid = info.m_clientid;
            this->m_port = info.m_port;
            this->m_keepalive = info.m_keepalive;
        }
        return *this;
    }  
    
    /*
    *
    */
    const std::string to_string(){
        return std::string("Host: [") + m_host + "] id: [" + m_clientid  +"] port: " + 
            std::to_string(m_port) + " keep alive: " + std::to_string(m_keepalive);
    }

private:    
    std::string m_clientid;
    std::string m_host;
    int m_port;
    int m_keepalive;
    int m_qos;

};

/*
* Interface for parent object
*/
class MqqtClientItf {
public:    
    MqqtClientItf() : 
        owner_notification(nullptr),
        m_err_connect(0),
        err_conn_max(3) {}

    virtual ~MqqtClientItf() {}

    virtual const int cl_connect(const MqqtServerInfo& conf) = 0;
    virtual const int cl_disconnect() = 0;
    virtual const std::string cl_get_version() const = 0;

    virtual void cl_notify(mqqt::MQQT_CLIENT_STATE state, mqqt::MQQT_CLIENT_ERROR code) const {
        if(owner_notification != nullptr){
            owner_notification(state, code);
        }
    } 

	std::function<void(MQQT_CLIENT_STATE state, MQQT_CLIENT_ERROR code)> owner_notification;

    const bool is_max_err_conn() const {
        return (m_err_connect >= err_conn_max);
    }
    
    void err_conn_inc() {
        m_err_connect++;
    }

private:    
    int m_err_connect; //connection error counter
    int err_conn_max;
};

/*
* MQQT client implementation
*/
template <class T>
class MqqtClient {
public:
    MqqtClient<T>(const MqqtServerInfo& conf);
    virtual ~MqqtClient();

    const int connect();
    const std::string get_version() const;
    const int publish(const std::string& topic, const std::string& payload);
    const int disconnect();

    void on_client(mqqt::MQQT_CLIENT_STATE state, mqqt::MQQT_CLIENT_ERROR code);

private:
    int m_mid;
    MqqtServerInfo m_conf;
    std::shared_ptr<T> m_mqqtCl;
};

} //end namespace mqqt

#endif