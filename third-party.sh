#!/bin/sh

if [ ! -d ../third-party ]
then
  mkdir ../third-party
fi

cd ../third-party

#
# Open SSL needed for Mosquitto
#
echo Open SSL
if [ ! -d ./openssl ]
then
  git clone https://github.com/openssl/openssl openssl
  cd ./openssl
  export set OPENSSL_ROOT_DIR=$PWD
  ./config
  make
  #make test
  cd ..
fi

#
#Mosquitto MQQT server
#
echo Moaquitto MQQT server
if [ ! -d ./mosquitto ]
then
  git clone http://github.com/eclipse/mosquitto mosquitto
  cd ./mosquitto
  cmake -H. -Bbuild
  make -C./build
  cd ..
fi

#
# WiringPi
#

echo WiringPi
if [ ! -d ./wiringPi ]
then
  #git clone git://git.drogon.net/wiringPi wiringPi
  wget -S "https://git.drogon.net/?p=wiringPi;a=snapshot;h=8d188fa0e00bb8c6ff6eddd07bf92857e9bd533a;sf=tgz" --output-document=wiringPi-8d188fa.tar.gz
  if [ $? -eq 0 ]; then
    tar zxvf ./wiringPi-8d188fa.tar.gz
    mv ./wiringPi-8d188fa ./wiringPi
    cd ./wiringPi
    ./build
    cd ..
  else
    echo Could not get WiringPi
  fi
fi

#
# Logger
#
echo SPDLOG
apt-get install libspdlog-dev

#
# JSON
#
echo jsoncons
if [ ! -d  ./jsoncons ]
then
  git clone https://github.com/danielaparker/jsoncons jsoncons
fi

if [ ! -d ./mongoose-5.1 ]
then
  wget -S "https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/mongoose/mongoose-5.1.tgz" --no-check-certificate
  tar zxvf ./mongoose-5.1.tgz
fi

#
# SQLite
#

if [ ! -d ./sqlite-amalgamation-3240000 ]
then
  wget https://www.sqlite.org/2018/sqlite-amalgamation-3240000.zip
  unzip sqlite-amalgamation-3240000.zip
fi

apt-get install libcurl4-openssl-dev --assume-yes

cd ../pi-robot

