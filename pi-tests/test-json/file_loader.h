/*
 * file_loader.h
 *
 * Service class for loading file in memory
 *
 * Created on: Jul 31, 2022
 *      Author: Denis Kudia
 */
#include <memory>
#include <fcntl.h>
#include <unistd.h>

#ifndef FILE_LOADER_H
#define FILE_LOADER_h

namespace piutils {
namespace floader {

/*
    Load file content to memory
*/

class Floader
{
public:
    Floader(const std::string& filename) : fname(filename) {
        fd = open(fname.c_str(), O_RDONLY);
        if(fd>0){
            fsize = lseek(fd, 0, SEEK_END);
            if(fsize>0){
                lseek(fd, 0, SEEK_SET);

                mfile = std::unique_ptr<char[]>(new char[fsize]);
                if(mfile){
                    ssize_t res = read(fd, mfile.get(), fsize);
                    if(res != fsize){
                        clear();
                    }
                }
            }
            close(fd);
        }
        result = errno;
    }

    const bool is_success() const {
        return fsize > 0;
    }

    const size_t get_size() const {
        return fsize;
    }

    virtual ~Floader(){
        clear();
    }

    const char * get() const {
        return mfile.get();
    }

    const std::string get_name() const{
        return fname;
    }

    const int get_err() const{
        return result;
    }

private:
    void clear(){
        if(mfile){
            mfile.release();
            fsize = 0;
        }
    }

    std::unique_ptr<char[]> mfile; 
    std::string fname;
    size_t fsize = 0;
    int fd = -1;
    int result = 0;
};

}//namespace floader
}//namespace piutils

#endif
