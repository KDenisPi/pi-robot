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
#include <ftw.h>
#include <list>

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

    void set_first_line(const std::string& fline){
        _first_line = fline;
    }

    /*
    * Collect list of files in a directory
    */
    static void collect_data_files(const std::string dir){
        int flags = 0;
        dfiles.clear();

        int res = nftw(dir.c_str(), finfo, 20, flags);
        if(res == -1){
            dfiles.clear();
        }
    }

    /*
    *
    */
    static int finfo(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
        //add files only
        if(tflag == FTW_F){
            dfiles.push_back(std::string(fpath));
        }
        return 0;
    }

    /*
    * Prepare file with list of available data files
    *
    * @parameters
    * fstor_path        - folder where data files are located
    * data_list_file    - full file name for list of files
    * first_line        - CSV header
    *
    */
    bool prepare_data_files_list(const std::string fstor_path, const std::string root_path, const std::string data_list_file, const std::string first_line) {
        int res = 0;
        ssize_t wr_bytes;

        //save path info
        _root_path = root_path;
        _data_list_file = data_list_file;

        //Prepare list of files
        piutils::fstor::FStorage::collect_data_files(fstor_path);

        // Override previous file
        int fd = open(data_list_file.c_str(), O_WRONLY|O_CREAT|O_APPEND|O_TRUNC, file_mode);
        if(fd == -1){
            res = errno;
            logger::log(logger::LLOG::ERROR, "fstor", std::string(__func__) + " Filed create file: " + data_list_file + " Error: " + std::to_string(res));
            return false;
        }
        else{
            //write first line
            if(!first_line.empty()){
                wr_bytes = write(fd, first_line.c_str(), first_line.length());
            }
        }

        for (std::string dfl : piutils::fstor::FStorage::dfiles) {
            std::string fdata = prepare_string_for_file_list(dfl, root_path);
            wr_bytes = write(fd, fdata.c_str(), fdata.length());
            logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__)+ " " + fdata);
        }
        close(fd);
        return true;
    }

    /*
    *
    */
    std::string prepare_string_for_file_list(const std::string dfl, const std::string root_path){
        std::string::size_type offset = dfl.rfind("/");
        std::string ffile = dfl.substr(offset+1);
        ffile.replace(4,1,"/");
        ffile.replace(6,1,"/");

        std::string fdata = ffile.substr(0, ffile.length() - 4) + "," + dfl.substr(root_path.length()+1) + "\n";
        return fdata;
    }

    /*
    * Append file with a line
    */
    bool append_data_files_list(const std::string dfl){
        int res = 0;
        ssize_t wr_bytes;

        logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__)+ " Add file: " + dfl);

        int fd = open(_data_list_file.c_str(), O_WRONLY|O_APPEND, file_mode);
        if(fd == -1){
            res = errno;
            logger::log(logger::LLOG::ERROR, "fstor", std::string(__func__) + " Filed create file: " + _data_list_file + " Error: " + std::to_string(res));
            return false;
        }

        std::string fdata = prepare_string_for_file_list(dfl, _root_path);

        logger::log(logger::LLOG::DEBUG, "fstor", std::string(__func__)+ " Append line: " + fdata);
        wr_bytes = write(fd, fdata.c_str(), fdata.length());

        close(fd);
        return true;
    }

    static std::list<std::string> dfiles;

private:
    std::string _names;

    std::string _dpath;             //path to the root storage folder (/var/data/pi-robot/data)
    std::string _root_path;         //root path for pi-robot data (/var/data/pi-robot)
    std::string _data_list_file;    //_root_path + /datafiles.csv
    std::string _curr_path;         //path to the current folder
    std::string _file_path;         //filename
    std::string _fext = ".csv";
    std::tm _time;
    bool _local_time;

    std::string _first_line;  //For CSV there are column headers
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
        bool new_file = false;

        create_filename();
        logger::log(logger::LLOG::INFO, "fstor", std::string(__func__) + " Trying create file: " + _file_path);

        if( access(_file_path.c_str(), F_OK) < 0 ){
            new_file = true;
        }

        //open data file
        _fd = open(_file_path.c_str(), O_WRONLY|O_CREAT|O_APPEND, file_mode);
        if(_fd == -1 ){
            res = errno;
            logger::log(logger::LLOG::ERROR, "fstor", std::string(__func__) + " Filed create file: " + _file_path + " Error: " + std::to_string(res));
        }
        else{
            //write first line
            if(!_first_line.empty()){
                logger::log(logger::LLOG::NECECCARY, "fstor", std::string(__func__) + " New file. Added first line");
                ssize_t wr_bytes = write(_fd, _first_line.c_str(), _first_line.length());
            }
        }

        append_data_files_list(_file_path);

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
