/*
 * statistics.h
 *
 *  Created on: May 8, 2018
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_STATISTICS_H_
#define PI_LIBRARY_STATISTICS_H_

namespace pirobot {
namespace stat{

//default counetrs and errors
enum StatCode {
    READ_BYTES = 0,
    READ_ERRORS,
    READ_SUCCESS,
    WRITE_SUCCESS,
    WRITE_ERRORS
};

//
//Item statistics (execution counters and errors)
//
class Statistics{
public:
    Statistics(unsigned int max_size=20) : _max_size(max_size) {
        _counters = new unsigned int[_max_size];
    }

    virtual ~Statistics() {
        delete[] _counters;
    }

    //increment counter by index
    inline const void inc(const unsigned int index){
        //TODO: Exception?
        if(index >= _max_size)
            return;
        
        _counters[index] += 1;
    }

    //increment counter by index
    inline const void add(const unsigned int index, const int value){
        //TODO: Exception?
        if(index >= _max_size)
            return;

        _counters[index] += value;
    }

    //get counter value
    const unsigned int get(const unsigned int index){
        //TODO: Exception?
        if(index >= _max_size)
            return 0;
        
        return _counters[index];
    }

    //increment counter by name
    inline const unsigned int inc(const std::string& index){
        return 0;
    }

    //get counter value
    const unsigned int get(const std::string& index){
        return 0;
    }
    
    //
    //Process some predefined statistic counters (read, write)
    //
    inline void read(const int status){
        inc((status < 0 ? StatCode::READ_ERRORS : StatCode::READ_SUCCESS));
    }

    inline void write(const int status){
        inc((status < 0 ? StatCode::WRITE_ERRORS : StatCode::WRITE_SUCCESS));
    }

private:
    unsigned int _max_size;
    unsigned int* _counters;
};

}//namespace stat
}//namespace pirobot

#endif //PI_LIBRARY_STATISTICS_H_
