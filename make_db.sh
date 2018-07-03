#!/bin/sh

#sudo apt-get install sqlite3 libsqlite3-dev

sqlite3 /var/data/pi-robot/weather.db
sqlite3 /var/data/pi-robot/weather.db < ./sql/create.sql


