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
./config
make
#make test
cd ..

#
#Mosquitto MQQT server 
#
echo Moaquitto MQQT server
git clone http://github.com/eclipse/mosquitto mosquitto
cd ./mosquitto
cmake -H. -Bbuild
make -C./build
cd ..

#
# WiringPi
#

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

#
#
#
echo jsoncons
git clone https://github.com/danielaparker/jsoncons jsoncons 

wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/mongoose/mongoose-5.1.tgz
tar zxvf ./mongoose-5.1.tgz

cd ../pi-robot

