/*
 * smallthings.h
 *
 *  Small utilites
 *
 *  Created on: Jun 19, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_SMALLTHINGS_H_
#define PI_LIBRARY_SMALLTHINGS_H_

#include <chrono>
#include <ctime>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "logger.h"

#if defined(__GNUC__)
    //  GCC
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
#else
    //  do nothing and hope for the best?
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif

namespace piutils {

/*
* Get current time on UTC or local
*/
void get_time(std::tm& result, std::time_t& time_now,const bool local_time = false);

/**
 * @brief Check if file exist and available
 *
 * @param fname
 * @param file_only - true check if path is regular file
 * @return true
 * @return false
 */
bool chkfile(const std::string& fname, const bool file_only = false);

/**
 * @brief If the file is regular file
 *
 * @param fname
 * @return true
 * @return false
 */
bool is_regular_file(const std::string& fname);

/*
* Return time string with format 'Mon, 29 Aug 2018 16:00:00 +1100'
* Used for email
*/
const std::string get_time_string(const bool local_time = true);

/*
*  Return list of files for directory
*  Size of list can be restricted by buffer size
*/
int get_dir_content(const std::string& dirname, std::string& result, const int max_result_size);

/*
* Trim spaces and CR LF
*/
const std::string trim(std::string& str);

/*
*
*/
void* map_memory_void(const uint32_t address, size_t len, const std::string& dev = "/dev/mem");

/*
* Map memory to the process address space
*/
template<class T>
T* map_memory(const uint32_t address, const std::string& dev = "/dev/mem"){
    void* result = map_memory_void(address, sizeof(T), dev);
    return static_cast<T*>(result);
}

/*
* Unmap memory
*/
void unmap_memory_void(void* address, size_t len);

template<class T>
T* unmap_memory(T* address){
    unmap_memory_void((void*)address, sizeof(T));
    return nullptr;
}

}//namespace piutils
#endif
