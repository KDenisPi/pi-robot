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
namespace piutils {

/*
* Get current time on UTC or local
*/
void get_time(std::tm& result, const bool local_time = false);

/*
* Check if file exist and available
*/
bool chkfile(const std::string& fname);

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
* Map memory to the process address space
*/
template<class T>
T* map_memory(const uint32_t address, const std::string& dev = "/dev/mem"){
    uint32_t page_size = getpagesize() - 1;
    uint32_t page_mask = ~0L ^ page_size;
    uint32_t offset = (address & page_size);

    int fd = open(dev.c_str(), O_RDWR | O_SYNC);
    if( fd < 0 ){
        logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " Could not open: " + dev + " Error: " + std::to_string(errno));
        return nullptr;
    }

    size_t len = sizeof(T);
    void* mem = mmap(0, len, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_LOCKED, fd, address & page_mask);

    std::cout << "Address: " << std::hex << address << " offset: " << std::hex << offset << " (address & page_mask) : " << (address & page_mask) << " Length: " << len << " Page mask: " << page_mask << std::endl;

    close(fd);

    if (mem == MAP_FAILED) {
        logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " mmap failed Error: " + std::to_string(errno));
        return nullptr;
    }

    std::cout << "Mem: " << std::hex << (uint32_t)mem << " (Mem+offset): " << ((uint32_t)mem + offset) << std::endl;

    return static_cast<T*>((void*)((char*)mem + offset));
}

template<class T>
T* unmap_memory(T* address){

    uint32_t page_size = getpagesize() - 1;
    uint32_t page_mask = ~0L ^ page_size;
    uintptr_t ptr = (uintptr_t)address & page_mask;

    int res = munmap( (void*)ptr, sizeof(T));
    if( res < 0 ){
        logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " munmap failed Error: " + std::to_string(errno));
    }

    return nullptr;
}

}
#endif
