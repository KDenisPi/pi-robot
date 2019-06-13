/*
 * core_memory.h
 *
 * Physical memory access
 *
 *  Created on: May 20, 2019
 *      Author: Denis Kudia
 */

#ifndef PI_CORE_PHYS_MEMORY_ACCESS_H_
#define PI_CORE_PHYS_MEMORY_ACCESS_H_

#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <memory>

#include "core_common.h"
#include "logger.h"

namespace pi_core {
namespace core_mem {

#define PAGEMAP_LENGTH 8
#define PAGEMAP_PAGE_FILE_PAGE  ((uint_fast64_t)1 << 61)
#define PAGEMAP_PAGE_SWAPPED    ((uint_fast64_t)1 << 62)
#define PAGEMAP_PAGE_PRESENT    ((uint_fast64_t)1 << 63)
/*
* Bits 55-63 flags
* Bits 0-54  page frame number (PFN) if present
*/
#define PAGEMAP_PAGE_FLAGS      ((uint_fast64_t)0x1FF << 55)

class PhysMemory;

/*
* First: memory map result
* Second: physical address for memory map result
*/
class MemInfo {
public:
    friend class PhysMemory;

    virtual ~MemInfo(){

    }

    /*
    *
    */
    const size_t get_size() const {
        return _len;
    }

    void* get_vaddr() const {
        return _vmemory;
    }

    uintptr_t get_paddr() const {
        return _phmemory;
    }

    const bool is_empty() const {
        return (_len== 0);
    }

protected:
    MemInfo(void* vmemory, uintptr_t phmemory, size_t len) :
        _vmemory(vmemory), _phmemory(phmemory), _len(len)
        {

        }

    void clear(){
        _vmemory = nullptr;
        _phmemory = 0L;
        _len = 0;
    }

    void* _vmemory;
    uintptr_t _phmemory;
    size_t _len;
};

/*
* Allocate memory in process address space and provide direct physical access to this memory
* Planned to be used for DMA operations
*/

class PhysMemory {
public:
    /*
    *
    */
    PhysMemory( const std::string& dev = "/proc/self/pagemap") : _fd(-1){
        _fd = open(dev.c_str(), O_RDONLY);
        if( _fd < 0 ){
            logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " Could not open: " + dev + " Error: " + std::to_string(errno));
            std::cout << "map_memory Could not open /proc/self/pagemap: "<< errno << std::endl;
        }

        _sdram_addr = pi_core::CoreCommon::get_sdram_address();
        std::cout << "map_memory Opend /proc/self/pagemap: "<< _fd << " SDRAM: " << std::hex << _sdram_addr << std::endl;
    }

    /*
    *
    */
    virtual ~PhysMemory(){
        std::cout << "~PhysMemory /proc/self/pagemap: "<< _fd << std::endl;
        _close();
    }

    bool is_good() const {
        return (_fd > 0);
    }

    /*
    *
    */
    const std::shared_ptr<MemInfo>  get_memory(const size_t len){
        void* mem = allocate_and_lock(len);
        if( !mem ){
            std::cout << "Failed to allocate: " << len << " bytes" << std::endl;
            return std::shared_ptr<MemInfo>();
        }

        uintptr_t ph_mem = virtual_to_physical(mem);
        if( !ph_mem){
            std::cout << "Failed to receive physical address" << std::endl;

            deallocate_and_unlock(mem, len);
            return std::shared_ptr<MemInfo>();
        }

        //std::cout << "get_memory Addr: 0x" << std::hex << ph_mem << " PyAddr: 0x" << std::hex << (ph_mem | _sdram_addr) << std::endl << std::endl;
        std::cout << "get_memory " << " PyAddr: 0x" << std::hex << ph_mem << std::endl << std::endl;
        //ph_mem |= _sdram_addr;

        return std::shared_ptr<MemInfo>(new MemInfo(mem, ph_mem, len));
    }

    /*
    *
    */
    void free_memory(std::shared_ptr<MemInfo>& minfo){
        if( deallocate_and_unlock(minfo->_vmemory, minfo->_len)){
            minfo.reset();
        }
    }

protected:
    /*
    *
    */
    void _close(){
        if(is_good()){
            close(_fd);
            _fd = -1;
        }
    }

    /*
    * Allogn buffer size to page
    */
    const size_t align_to_page_size(const size_t len){
        uint32_t page_size = getpagesize();
        uint32_t modulo = (len & (page_size-1));
        uint32_t page_mask = ~0L ^ (page_size-1);
        size_t result = len;

        if( modulo > 0){
            result =  (len & page_mask) + page_size;
        }

        std::cout << "align_to_page_size page size: " << std::dec << page_size << " In: " << len << " Out: " << result << " Modulo: " << modulo << std::endl;

        return result;
    }

    /*
    * Map memoty in process address space and lock memory allocation
    */
    void* allocate_and_lock(const size_t len){

#ifdef _USE_VALLOC_
        size_t len_page = len;
        void* mem = valloc(len);
#else
        size_t len_page = align_to_page_size(len);
        void* mem = mmap(NULL, len_page, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
#endif

#ifdef _USE_VALLOC_
        if (mem == NULL) {
#else
        if (mem == MAP_FAILED) {
#endif
            logger::log(logger::LLOG::ERROR, "phys_mem", std::string(__func__) + " mmap failed Error: " + std::to_string(errno));
            std::cout << "allocate_and_lock Failed to allocate MMAP:" << errno << std::endl;
            return nullptr;
        }

        ((char*)mem)[0] = 'D';
        mlock(mem, len_page);

        memset(mem, 0, len_page);

        std::cout << "allocate_and_lock Success Allocated: " << std::dec << len_page << "[" << len << "] Address " << std::hex << mem << std::endl << std::endl;
        return mem;
    }

    /*
    * Unmap memory
    */
    bool deallocate_and_unlock(void* address, const size_t len){
       bool result = true;
       std::cout << "deallocate_and_unlock Len: " << std::dec << len << " Address " << std::hex << address << std::endl;

#ifdef _USE_VALLOC_
       size_t len_page = len;
#else
       size_t len_page = align_to_page_size(len);
#endif
       munlock(address, len_page);

#ifdef _USE_VALLOC_
      free(address);
#else
      int res = munmap((void*)address, len_page);
      if(res == -1){
           logger::log(logger::LLOG::ERROR, "phys_mem", std::string(__func__) + " munmap failed Error: " + std::to_string(errno));
           result = false;
      }
#endif
      std::cout << "deallocate_and_unlock Success Len: " << std::dec << len_page << " Address " << std::hex << address << std::endl;
      return result;
    }

    /*
    * Detect real physical address for memory from user address space
    *

    https://www.kernel.org/doc/Documentation/vm/pagemap.txt

    pagemap, from the userspace perspective
    ---------------------------------------

    pagemap is a new (as of 2.6.25) set of interfaces in the kernel that allow
    userspace programs to examine the page tables and related information by
    reading files in /proc.

    There are four components to pagemap:

    /proc/pid/pagemap.  This file lets a userspace process find out which
    physical frame each virtual page is mapped to.  It contains one 64-bit
    value for each virtual page, containing the following data (from
    fs/proc/task_mmu.c, above pagemap_read):

        Bits 0-54  page frame number (PFN) if present
        Bits 0-4   swap type if swapped
        Bits 5-54  swap offset if swapped
        Bit  55    pte is soft-dirty (see Documentation/vm/soft-dirty.txt)
        Bit  56    page exclusively mapped (since 4.2)
        Bits 57-60 zero
        Bit  61    page is file-page or shared-anon (since 3.5)
        Bit  62    page swapped
        Bit  63    page present

    Since Linux 4.0 only users with the CAP_SYS_ADMIN capability can get PFNs.
    In 4.0 and 4.1 opens by unprivileged fail with -EPERM.  Starting from
    4.2 the PFN field is zeroed if the user does not have CAP_SYS_ADMIN.
    Reason: information about PFNs helps in exploiting Rowhammer vulnerability.

    If the page is not present but in swap, then the PFN contains an
    encoding of the swap file number and the page's offset into the
    swap. Unmapped pages return a null PFN. This allows determining
    precisely which pages are mapped (or in swap) and comparing mapped
    pages between processes.

    Efficient users of this interface will use /proc/pid/maps to
    determine which areas of memory are actually mapped and llseek to
    skip over unmapped regions.

    /proc/kpagecount.  This file contains a 64-bit count of the number of
    times each page is mapped, indexed by PFN.

    /proc/kpageflags.  This file contains a 64-bit set of flags for each
    page, indexed by PFN.
    */

    uintptr_t virtual_to_physical(void* vaddress) {
        uintptr_t result = 0L;
        uint_fast64_t  bit_page_present = ((uint_fast64_t)1 << 63);

        int psize = getpagesize();
        if( !is_good() ){
            logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + "Error. Not initialized");
            std::cout << "virtual_to_physical Not initialized " << std::endl;
            return result;
        }

        std::cout << "virtual_to_physical Addr: " << std::hex << vaddress << " psize: " << std::dec << psize << std::endl << std::endl;

        //detect page number
        uintptr_t page_number = (uintptr_t)(vaddress)/psize;
        //detect offset on the page
        int byteOffsetFromPage = (uintptr_t)(vaddress)%psize;

        //find physical page descriptor
        off_t offset = page_number*PAGEMAP_LENGTH;
        int r_off = lseek(_fd, offset, SEEK_SET);
        if(r_off == offset){
            /*
            * Read page descriptor
            */
            uint64_t phys_page;
            if( read(_fd, &phys_page, PAGEMAP_LENGTH) < 0){
                logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " Could not read page information: Error: " + std::to_string(errno));

                std::cout << "virtual_to_physical Could not read page information: Error: " << errno << std::endl;
            }
            else{
                //check if page present
                if((phys_page & PAGEMAP_PAGE_PRESENT) != 0){
                    /*
                    * Calculate real physical address
                    */
                    phys_page = phys_page & (~PAGEMAP_PAGE_FLAGS);
                    result = (uintptr_t)(phys_page*psize + byteOffsetFromPage);

                    std::cout << "virtual_to_physical Page: " << std::dec << phys_page << " Address: " << std::hex << std::endl;
                }
                else{
                    logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " Page not present in memory");
                    std::cout << "virtual_to_physical Page not present in memory: " << std::endl;
                }
            }

        }
        else{
            logger::log(logger::LLOG::ERROR, "map_memory", std::string(__func__) + " Could not seek to page: Error: " + std::to_string(errno));
            std::cout << "virtual_to_physical Could not seek to page: Error: " << errno << std::endl;
        }

        return result;
    }

private:
    int _fd; //memory file descriptor
    uintptr_t _sdram_addr;
};

} //namespace core_mem
} //namespace pi_core

#endif
