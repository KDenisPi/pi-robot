/*
 * MqqtClient.cpp
 *
 *  Created on: Apr 17, 2017
 *      Author: Denis Kudia
 */
#include <functional>

#include <logger.h>
#include "MqqtClient.h"

namespace mqqt {

const char TAG[] = "mqqtcl";

/*
*
*/
template<class T>
MqqtClient<T>::MqqtClient(const MqqtServerInfo& conf) : 
    m_mid(0),
    m_conf(conf)
{
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Client ID: " + (NULL == m_conf.clientid() ? std::string("NotDefined") : m_conf.clientid()));
    m_mqqtCl = std::shared_ptr<T>(new T(m_conf.clientid()));
    m_mqqtCl->owner_notification = std::bind(&MqqtClient::on_client, this, std::placeholders::_1, std::placeholders::_2);
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Version: " + get_version());   
}

/*
*
*/
template<class T>
MqqtClient<T>::~MqqtClient(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));
    if(m_mqqtCl){
        m_mqqtCl->cl_disconnect();
    }
    m_mqqtCl.reset();
}



/*
*
*/
template<class T>
const int MqqtClient<T>::connect(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__));
    return m_mqqtCl->cl_connect(m_conf);
}

/*
*
*/
template<class T>
const std::string MqqtClient<T>::get_version() const {
    return m_mqqtCl->get_version();
}


} //end namespace mqqt