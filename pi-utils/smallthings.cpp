/*
 * smallthings.cpp
 *
 *  Small utilites
 *
 *  Created on: Sep 4, 2018
 *      Author: Denis Kudia
 */

#include "smallthings.h"
namespace piutils {

/*
* Check if file exist and available
*/
bool chkfile(const std::string& fname){
    int res = access(fname.c_str(), F_OK);
    return (res == 0);
}

//
// Get current time on UTC or local
//
void get_time(std::tm& result, std::time_t& time_now, const bool local_time){
    std::chrono::time_point<std::chrono::system_clock> tp;
    tp = std::chrono::system_clock::now();
    time_now = std::chrono::system_clock::to_time_t(tp);

    //TODO Add semaphore
    std::tm* tm = (local_time ? std::localtime(&time_now) : std::gmtime(&time_now));
    std::memcpy(&result, tm, sizeof(std::tm));
}

//
//Return time string with format 'Mon, 29 Aug 2018 16:00:00 +1100'
// Used for email
//
const std::string get_time_string(const bool local_time){
  std::chrono::time_point<std::chrono::system_clock> tp;
  char mtime[128];

  tp = std::chrono::system_clock::now();
  std::time_t time_now = std::chrono::system_clock::to_time_t(tp);
  if(std::strftime(mtime, sizeof(mtime), "%a, %d %b %Y %H:%M:%S %z", (local_time ? std::localtime(&time_now) : std::gmtime(&time_now))))
    return std::string(mtime);

  //Exception?
  return std::string();
}

/*
*  Return list of files for directory
*  Size of list can be restricted by buffer size
*/
int get_dir_content(const std::string& dirname, std::string& result, const int max_result_size){
    int res = 0;

    DIR *pdir = opendir( dirname.c_str());
    if( pdir == NULL ){
      return errno;
    }

    for(;;){
      errno = 0;

      struct dirent *dp = readdir( pdir );
      if( dp == NULL ){
        res = errno; // errno - 0 - end of list, errno > 0 - error
        break;
      }

      if( strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 )
        continue;

      // check length  for output buffer
      if( max_result_size > 0 && (result.length() + strlen(dp->d_name) + 5) >= max_result_size){
        res = -1;
        break;
      }

      if( dp->d_type == DT_DIR ) //directory
        result += "[D] " + std::string(dp->d_name) + "\n";
      else if(dp->d_type == DT_REG ) //other types - do not separate
        result += "[F] " +std::string(dp->d_name) + "\n";
      else
        result += "[ ] " + std::string(dp->d_name) + "\n";
    }

    closedir( pdir );
    return res;
}

/*
* Trim spaces and CR LF
*/
const std::string trim(std::string& str){
    int first = 0;
    int last = str.length();

    for ( std::string::iterator it = str.begin(); it != str.end(); ++it){
      if( *it == ' '){
        first++;
        continue;
      }
      break;
    }

    for ( std::string::iterator it = str.end(); it != str.begin(); --it){
      if( *it == ' ' || *it == 0x0D || *it == 0x0A ) {
        last--;
        continue;
      }
      break;
    }

    return str.substr( first, last - first);
}

/*
*
*/

/*
* Map memory to the process address space
*/
void* map_memory_void(const uint32_t address, size_t len, const std::string& dev){
    uint32_t page_size = getpagesize() - 1;
    uint32_t page_mask = ~0L ^ page_size;
    uint32_t offset = (address & page_size);

    int fd = open(dev.c_str(), O_RDWR | O_SYNC);
    if( fd < 0 ){
        logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " Could not open: " + dev + " Error: " + std::to_string(errno));
        return nullptr;
    }

    void* mem = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, address & page_mask);

    logger::log(logger::LLOG::DEBUG, "map_memory", std::string(__func__) + "Address: " + std::to_string(address) +
        " offset: " + std::to_string(offset) + " (address & page_mask) : " + std::to_string((address & page_mask)) + " Length: " + std::to_string(len) +
        " Page mask: " + std::to_string(page_mask));

    close(fd);

    if (mem == MAP_FAILED) {
        logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " mmap failed Error: " + std::to_string(errno));
        return nullptr;
    }

    return (void*)((char*)mem + offset);
}

/*
* Unmap memory
*/
void unmap_memory_void(void* address, size_t len){
    uint32_t page_size = getpagesize() - 1;
    uint32_t page_mask = ~0L ^ page_size;
    uintptr_t ptr = (uintptr_t)address & page_mask;

    int res = munmap( (void*)ptr, len);
    if( res < 0 ){
        logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " munmap failed Error: " + std::to_string(errno));
    }
}

}
