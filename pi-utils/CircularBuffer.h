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

namespace piutils {
namespace circbuff {

template<typename T>
class CircularBuffer {
public:
    /*
    *
    */
    CircularBuffer(const std::size_t max_size = 100) : max_size_(max_size){
        buffer = new T[max_size];
    }

    /*
    *
    */
    ~CircularBuffer(){
        delete[] buffer;
    }

    /*
    *
    */
    const void put(const T& value){
        std::lock_guard<std::mutex> lk(cv_m);
        buffer[head_] = value;
        head_ = (head_ + 1 >= max_size_ ? 0 : head_+1);
        if(head_ == tail_)
            tail_ = (tail_ + 1 >= max_size_ ? 0 : tail_+1);
    }

    /*
    *
    */
    const T& get(){
        std::lock_guard<std::mutex> lk(cv_m);
        if(!is_empty()){
            m_last_value = buffer[tail_];
            tail_ = (tail_ + 1 >= max_size_ ? 0 : tail_+1);
        }

        return m_last_value;
    }

    /*
    * Empty buffer
    */
    const void empty(){
        std::lock_guard<std::mutex> lk(cv_m);
        tail_ = head_;
    }

    /*
    * Check if buffer is empty
    */
    const bool is_empty() const{
        return (tail_ == head_);
    }

private:
    T* buffer;
    T m_last_value;
    int tail_ = 0;
    int head_ = 0;
    int max_size_;
    std::mutex cv_m;	
};

}
}
 #endif