/*
 * MqqtDefines.h
 *
 *  Created on: Apr 25, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQQT_DEFINES_H_
#define PI_MQQT_DEFINES_H_

namespace mqqt {

enum MQQT_CLIENT_STATE {
    MQQT_CONNECT = 0,
    MQQT_DISCONNECT = 1,
    MQQT_PUBLISH = 2,
    MQQT_SUBSCRIBE = 3
};

enum MQQT_CLIENT_ERROR {
    MQQT_ERROR_SUCCESS = 0,
    MQQT_ERROR_INVAL = 1,
    MQQT_ERROR_FAILED = 2
};


} //end namespace mqqt
#endif
