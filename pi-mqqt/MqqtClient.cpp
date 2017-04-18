/*
 * MqqtClient.cpp
 *
 *  Created on: Apr 17, 2017
 *      Author: Denis Kudia
 */
#include <logger.h>
#include "MqqtClient.h"

namespace mqqt {

const char TAG[] = "mqqtcl";

/*
*
*/
template<class T>
MqqtClient<T>::MqqtClient(const char* clientID){
    logger::log(logger::LLOG::NECECCARY, TAG, std::string(__func__) + " Client ID: " + (clientID==NULL ? std::string("NotDefined") : clientID));
    m_mqqtCl = std::shared_ptr<T>(new T(clientID));
}


} //end namespace mqqt