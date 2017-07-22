#!/bin/sh

mkdir ../third-party
cd ../third-party
#
#Mosquitto MQQT server 
#
echo Moaquitto MQQT server
git clone http://github.com/eclipse/mosquitto mosquitto
cd ./mosquitto
cmake -H. -Bbuild
make -C./build

#
# WiringPi
#
cd ..
echo WiringPi
git clone git://git.drogon.net/wiringPi wiringPi
cd ./wiringPi
./build

cd ../../pi-robot

