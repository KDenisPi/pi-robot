/*
 * MqttDefines.h
 *
 *  Created on: Apr 25, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQTT_DEFINES_H_
#define PI_MQTT_DEFINES_H_

namespace mqtt {

enum MQTT_CLIENT_STATE {
    MQTT_CONNECT = 0,
    MQTT_DISCONNECT = 1,
    MQTT_PUBLISH = 2,
    MQTT_SUBSCRIBE = 3
};

enum MQTT_CLIENT_ERROR {
    MQTT_ERROR_SUCCESS = 0,
    MQTT_ERROR_INVAL = 1,
    MQTT_ERROR_FAILED = 2
};


} //end namespace mqtt
#endif
