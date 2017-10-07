/*
 * MosquittoClient.h
 *
 *  Created on: Apr 14, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQQT_MOSQUITTO_CL_H_
#define PI_MQQT_MOSQUITTO_CL_H_

#include <functional>
#include <mosquittopp.h>

#include "MqqtDefines.h"
#include "MqqtClientItf.h"

namespace mqqt {

class MosquittoClient : public mosqpp::mosquittopp, public MqqtClientItf
{
public:       
    MosquittoClient(const char* clientID);
    virtual ~MosquittoClient();

    virtual const int cl_connect(const MqqtServerInfo& conf);
    virtual const int cl_disconnect();
    virtual const std::string cl_get_version() const;
    virtual const int cl_publish(int mid, std::string& topic, std::string& payload) override;
	

	virtual void on_connect(int /*rc*/);
	virtual void on_disconnect(int /*rc*/);
	virtual void on_publish(int /*mid*/);
	virtual void on_message(const struct mosquitto_message * /*message*/){return;}
	virtual void on_subscribe(int /*mid*/, int /*qos_count*/, const int * /*granted_qos*/);
	virtual void on_unsubscribe(int /*mid*/);
	virtual void on_log(int /*level*/, const char * /*str*/);
	virtual void on_error();    

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

bool const is_connected() const {return m_connected;}	

private:
	int m_qos;
	bool m_connected;
};

} /*end namespace mqqt*/

#endif