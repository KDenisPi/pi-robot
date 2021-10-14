#!/bin/sh

/home/denis/third-party/mosquitto/build/client/mosquitto_sub -h dk-centos7-mqtt.localdomain -t stm -p 8883 --cafile /home/denis/pi-robot/mqtt_tls/m2mqtt_ca.crt

