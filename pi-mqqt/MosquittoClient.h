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
#include "MqqtClient.h"

namespace mqqt {

class MosquittoClient : public mosqpp::mosquittopp, public MqqtClientItf
{
public:       
    MosquittoClient(const char* clientID);
    virtual ~MosquittoClient() {}

    virtual const int cl_connect(const MqqtServerInfo& conf);
    virtual const int cl_disconnect();
    virtual const std::string cl_get_version() const;

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
};

} /*end namespace mqqt*/

#endif