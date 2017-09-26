#!/bin/sh

mkdir ../third-party
cd ../third-party

#
# Open SSL needed for Mosquitto
#
echo Open SSL
git clone https://github.com/openssl/openssl openssl
cd ./openssl
export set OPENSSL_ROOT_DIR=$PWD
make
#make test

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

#
#
#
cd ..
echo SPDLOG 
git clone https://github.com/gabime/spdlog.git spdlog


cd ../pi-robot

