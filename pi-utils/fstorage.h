/*
 * fstorage.h
 *
 *  Simple implemnetation for file based data storage
 *
 *  Created on: Jun 19, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_FSTORAGE_H_
#define PI_LIBRARY_FSTORAGE_H_

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <chrono>
#include <ctime>
#include <cstring>

#include "smallthings.h"
#include "logger.h"

namespace piutils {
namespace fstor {

//
// Simple implemnetation for file based data storage
//
class FStorage {
public:
    //
    //
    //
    FStorage() :
        _dpath(""), _local_time(false), _fd(-1)
    {
        logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__) + " Path: " + _dpath + " Local time: " + std::to_string(_local_time));
    }

    //
    //
    //
    virtual ~FStorage() {
        logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__));
        close_file();
    }

    //
    // Create initial infrastructure
    //
    int initilize(const std::string& dpath = "/var/data/pi-robot/data", const bool use_local_time = false){
        logger::log(logger::LLOG::INFO, "fstor", std::string(__func__) + " Path: " + _dpath + " Local time: " + std::to_string(_local_time));

        _local_time = use_local_time;
        if(!dpath.empty())
            _dpath = dpath;

        if(_dpath.empty()){
            logger::log(logger::LLOG::ERROR, "fstor", std::string(__func__) + " Failed. Destination path is not defined.");
            return -1;
        }

        //initilize time structure
        piutils::get_time(_time);

        //check if root data folder exist
        int res = check_path(_dpath);
        if(res == 0){
            //create path for currnet month
            res = create_month_dir(1900 + _time.tm_year, _time.tm_mon + 1, _curr_path);
            if( res == 0){
                res = create_file();
            }
        }

        logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__) + " Result: " + std::to_string(res));
        return res;
    }

    //
    // Write data to file
    //
    const int write_data(const char* data, size_t dsize) {
        int res = 0;

        //check file descriptor
        if(_fd < 0){
            //TODO: remove for PRO
            logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__) + " File closed.");
            return -1;
        }

        //check if date changed
        std::tm curr_time;
        piutils::get_time(curr_time);

        //if need to create a new folder
        if((curr_time.tm_year != _time.tm_year) || (curr_time.tm_mon  != _time.tm_mon)){
            res = create_month_dir(1900 + curr_time.tm_year, curr_time.tm_mon + 1, _curr_path);
        }

        //Need to create another file
        if(curr_time.tm_mday != _time.tm_mday){
            memcpy(&_time, &curr_time, sizeof(std::tm));
            close_file();
            res = create_file();
            if(res != 0)
                return res;
        }

        ssize_t wr_bytes = write(_fd, data, dsize);
        if(wr_bytes == -1){
            res = errno;
            logger::log(logger::LLOG::ERROR, "fstor", std::string(__func__) + " Error: " + std::to_string(res));
        }
        else if(wr_bytes != dsize){
            logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__) + " Different size");
        }

        return res;
    }


    //
    // Stop
    //
    void stop(){
        logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__));

        close_file();
    }

private:
    std::string _dpath;     //path to the root storage folder
    std::string _curr_path; //path to the current folder
    std::string _file_path; //filename
    std::string _fext = ".csv";
    std::tm _time;
    bool _local_time;

    int _fd;

    //
    // Close
    //
    inline int close_file(){
        logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__));

        if(_fd){
            //TODO: Add flush

            close(_fd);
            _fd = -1;
        }
    }

    //
    //
    //
    inline void create_filename(){
        _file_path = _curr_path + "/" + std::to_string(1900 + _time.tm_year) + "_" + std::to_string(_time.tm_mon + 1) + "_" + std::to_string(_time.tm_mday) + _fext;
    }

    //
    //
    //
    int create_month_dir(const int year, const int month, std::string& mdir){
        mdir = _dpath + "/" + std::to_string(year) + "_" + std::to_string(month);
        return create_folder(mdir);
    }

    //
    //
    //
    inline int create_file(){
        int res  = 0;

        create_filename();
        logger::log(logger::LLOG::INFO, "fstor", std::string(__func__) + " Trying create file: " + _file_path);

        //open data file
        _fd = open(_file_path.c_str(), O_WRONLY|O_CREAT|O_APPEND, file_mode);
        if(_fd == -1 ){
            res = errno;
            logger::log(logger::LLOG::ERROR, "fstor", std::string(__func__) + " Filed create file: " + _file_path + " Error: " + std::to_string(res));
        }
        return res;
    }

    //
    //
    //
    int check_path(const std::string& dpath){
        logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__) + " Path: " + dpath);

        int res = 0;
        size_t pos = dpath.find_first_of("/", 0);
        while(pos != std::string::npos){
            if(pos > 0){
                const std::string sub_path = dpath.substr(0, pos);
                res = create_folder(sub_path);
                if(res != 0)
                    return res;
            }
            pos = dpath.find_first_of("/", pos + 1);
        }
        res = create_folder(dpath);

        return res;
    }

    //
    //
    //
    int create_folder(const std::string& sub_path){
        logger::log(logger::LLOG::INFO, "fstor", std::string(__func__) + " Folder: " + sub_path);

        //check if parent folder exist
        int res = access(sub_path.c_str(), F_OK);
        if(res < 0){
            //if not then try to create it
            res = mkdir(sub_path.c_str(), dir_mode);
            //could not create folder - return error
            if(res < 0){
                res = errno;
                logger::log(logger::LLOG::ERROR, "fstor", std::string(__func__) + " Could not create folder : " + sub_path +
                    " Error: " + std::to_string(res));
            }
        }
        return res;
    }

    //file mode
    mode_t file_mode = S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH;
    // directory mode
    mode_t dir_mode = S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH;
};

}//namespace fstor
}//namespace piutils

#endif