/*
 * MqqtClient.h
 *
 *  Created on: Apr 17, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQQT_CLIENT_H_
#define PI_MQQT_CLIENT_H_

#include <memory>

namespace mqqt {

template <class T>
class MqqtClient {
public:
    MqqtClient<T>(const char* clientID);
    virtual ~MqqtClient() {}

    std::shared_ptr<T> m_mqqtCl;
};

} //end namespace mqqt

#endif