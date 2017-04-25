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
    MqqtServerInfo(const std::string host) : 
        m_host(host), m_clientid(""), m_port(1883), m_keepalive(60) {

    }

    MqqtServerInfo(const std::string clientid, const std::string host) : 
        m_host(host), m_clientid(clientid), m_port(1883), m_keepalive(60) {

    }
    
    virtual ~MqqtServerInfo() {}

    bool is_empty() const {return m_host.empty();}
    const char* host() const {return m_host.c_str();}

    const char* clientid() const {return (m_clientid.empty() ? NULL : m_clientid.c_str());}
    
    void set_port(const int port) {m_port = port;}
    const int port() const { return m_port;}
    
    void set_keepalive(const int keepalive){ m_keepalive = keepalive;}
    const int keepalive()const {return m_keepalive;}

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

};

/*
* Interface for parent object
*/
class MqqtClientItf {
    virtual const int connect() = 0;
    virtual const std::string get_version() const = 0;
};

/*
* MQQT client implementation
*/
template <class T>
class MqqtClient : public MqqtClientItf{
public:
    MqqtClient<T>(const MqqtServerInfo& conf);
    virtual ~MqqtClient();

    virtual const int connect();
    virtual const std::string get_version() const;

    void on_client(mqqt::MQQT_CLIENT_STATE state, mqqt::MQQT_CLIENT_ERROR code);

private:
    int m_mid;
    MqqtServerInfo m_conf;
    std::shared_ptr<T> m_mqqtCl;
};

} //end namespace mqqt

#endif