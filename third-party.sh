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
#Mosquitto mqtt server
#
#
# git clone -b 1.6.x https://github.com/eclipse/mosquitto.git mosquitto
#

echo Moaquitto mqtt server
if [ ! -d ./mosquitto ]
then
  git clone http://github.com/eclipse/mosquitto mosquitto
  cd ./mosquitto
  cmake -H. -Bbuild
  make -C./build
  cd ..
fi

#
# Logger
#
# git clone https://github.com/gabime/spdlog.git -b v1.x
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


#
# Web Server
#

#
# Use GitHub git clone -b 7.4 https://github.com/cesanta/mongoose
#
if [ ! -d ./mongoose-5.1 ]
then
  wget -S "https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/mongoose/mongoose-5.1.tgz" --no-check-certificate
  tar zxvf ./mongoose-5.1.tgz
fi

#
# SQLite
#
#
# Use GitHub git clone -b v3.8.10 https://github.com/azadkuh/sqlite-amalgamation.git sqlite-amalgamation
#

if [ ! -d ./sqlite-amalgamation-3240000 ]
then
  wget https://www.sqlite.org/2018/sqlite-amalgamation-3240000.zip
  unzip sqlite-amalgamation-3240000.zip
fi

apt-get install libcurl4-openssl-dev --assume-yes

cd ../pi-robot

curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | sudo gpg --dearmor -o /usr/share/keyrings/githubcli-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | sudo tee /etc/apt/sources.list.d/github-cli.list > /dev/null
sudo apt update
sudo apt install gh

gh auth login --with-token < token.txt