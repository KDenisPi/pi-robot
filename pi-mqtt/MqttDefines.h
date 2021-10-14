/*
 * mqttDefines.h
 *
 *  Created on: Apr 25, 2017
 *      Author: Denis Kudia
 */
#ifndef PI_MQTT_DEFINES_H_
#define PI_MQTT_DEFINES_H_

namespace mqtt {

enum mqtt_CLIENT_STATE {
    mqtt_CONNECT = 0,
    mqtt_DISCONNECT = 1,
    mqtt_PUBLISH = 2,
    mqtt_SUBSCRIBE = 3
};

enum mqtt_CLIENT_ERROR {
    mqtt_ERROR_SUCCESS = 0,
    mqtt_ERROR_INVAL = 1,
    mqtt_ERROR_FAILED = 2
};


} //end namespace mqtt
#endif
