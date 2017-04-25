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
    if(m_mqqtCl){
        m_mqqtCl->disconnect();
    }
    m_mqqtCl.reset();
}

/*
*
*/
template<class T>
void MqqtClient<T>::on_client(mqqt::MQQT_CLIENT_STATE state, mqqt::MQQT_CLIENT_ERROR code){

}


/*
*
*/
template<class T>
const int MqqtClient<T>::connect(){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__));
    return m_mqqtCl->connect_async(m_conf.host(), m_conf.port(), m_conf.keepalive());
}

/*
*
*/
template<class T>
const std::string MqqtClient<T>::get_version() const{
    int mjr=0, mnr=0, rev=0;
    m_mqqtCl->lib_version(&mjr, &mnr, &rev);
    return std::to_string(mjr) + "." + std::to_string(mnr) + "." + std::to_string(rev);
}


} //end namespace mqqt