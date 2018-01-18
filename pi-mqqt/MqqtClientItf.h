/*
 * MqqtClientItf.h
 *
 *  Created on: Jul 3, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQQT_CLIENTITF_H_
#define PI_MQQT_CLIENTITF_H_

#include "MqqtDefines.h"
#include "MqqtServerInfo.h"

namespace mqqt {

/*
* Interface for client implementation
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
    virtual const int cl_publish(int mid, std::string& topic, std::string& payload) = 0;

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

} //end namespace mqqt

#endif