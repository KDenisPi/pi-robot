/*
 * CircularBuffer.h
 *
 *  Created on: Sep 19, 2017
 *      Author: Denis Kudia
 */

 #ifndef PI_LIBRARY_CIRCULARBUFFER_H_
 #define PI_LIBRARY_CIRCULARBUFFER_H_

#include <vector>
#include <mutex>

namespace putils {
namespace circbuff {

template<typename T>
class CircularBuffer {
public:
    /*
    *
    */
    CircularBuffer(const std::size_t count) {
        buffer = std::vector<T>(count);
    }

    /*
    *
    */
    ~CircularBuffer(){

    }

    /*
    *
    */
    const void put(const T& value){
        std::lock_guard<std::mutex> lk(cv_m);
        if(buffer.size() == buffer.max_size())
            buffer.erase(buffer.begin());
        buffer.push_back(value);        
    }

    /*
    *
    */
    const T& get(){
        std::lock_guard<std::mutex> lk(cv_m);
        if(!buffer.empty()){
            m_last_value = buffer.front();
            buffer.erase(buffer.begin());
        }

        return m_last_value;
    }

    const bool is_empty() const{
        return buffer.empty();
    }

private:
    std::vector<T> buffer;
    T m_last_value;

    std::mutex cv_m;	
};

}
}
 #endif