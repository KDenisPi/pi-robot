/*
 * sqlite.h
 *
 *  SQL Lite DB wrapper
 *
 *  Created on: Jul 04, 2018
 *      Author: Denis Kudia
 */

#ifndef PISQL_SQLITE_H_
#define PISQL_SQLITE_H_

#include "sqlite3.h"
#include <string>

#include "logger.h"

namespace pisql {

class Sqlite {
public:

    /*
    *
    */
    Sqlite(const std::string& database = "") : _database(database) {

    }

    /*
    *
    */
    virtual ~Sqlite(){
        if(_pDb){
            disconnect();
        }
    }

    /*
    *
    */
    const bool connect(const std::string& database = ""){

        if(!database.empty())
            _database = database;

        logger::log(logger::LLOG::INFO, "sqldb", std::string(__func__) + " Connect to: " + _database);

        int res = sqlite3_open_v2(_database.c_str(), &_pDb, SQLITE_OPEN_READWRITE, nullptr);
        if(res != SQLITE_OK){
            logger::log(logger::LLOG::ERROR, "sqldb", std::string(__func__) + " Open failed : " + std::to_string(res));
        }

        return (res == SQLITE_OK);
    }

    sqlite3 * db() const {
        return _pDb;
    }
    /*
    *
    */
    const bool disconnect(){
        int res = sqlite3_close_v2(_pDb);
        if(res == SQLITE_OK){
            _pDb = nullptr;
        }
        logger::log(logger::LLOG::ERROR, "sqldb", std::string(__func__) + " Result : " + std::to_string(res));
        return (res == SQLITE_OK);
    }

private:
    sqlite3 *_pDb = nullptr;
    std::string _database;
};

} //namespace pisql

#endif
