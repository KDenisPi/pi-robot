/*
  //CircularBuffer.h

   Created on: Sep 19, 2017
       Author: Denis Kudia
*/

 #ifndef PI_LIBRARY_CIRCULARBUFFER_H_
 #define PI_LIBRARY_CIRCULARBUFFER_H_

#include <vector>
#include <mutex>

#include <iostream>

namespace piutils {
namespace circbuff {

template<typename T>
class CircularBuffer {
public:
    /*
    *
    */
    CircularBuffer(const std::size_t max_size = 100) : max_size_(max_size), m_size(0) {
        buffer = new T[max_size];
        //std::cout << "CircularBuffer " + std::string(__func__) + " Constructor" << std::endl;
    }

    /*
    *
    */
    ~CircularBuffer(){
        delete[] buffer;
        //std::cout << "CircularBuffer " + std::string(__func__) + " Destructor" << std::endl;
    }


    /*
    *
    */
    const void put(const T& value){
        //std::cout << "CircularBuffer " + std::string(__func__) + " Put 1 started" << " Size: " << std::to_string(m_size) << std::endl;
        std::lock_guard<std::mutex> lk(cv_m);

        if( m_size == max_size_){
            //std::cout << "Full 1 Size: " << std::to_string(m_size) << std::endl;
            return;
        }

        buffer[head_] = value;
        head_ = (head_ + 1 >= max_size_ ? 0 : head_+1);
        if(head_ == tail_)
            tail_ = (tail_ + 1 >= max_size_ ? 0 : tail_+1);
        m_size++;
    }
    /*
    *
    */
    const void put(T&& value){
        //std::cout << "CircularBuffer " + std::string(__func__) + " Put 2 started" << " Size: " << std::to_string(m_size) << std::endl;
        std::lock_guard<std::mutex> lk(cv_m);

        if( m_size == max_size_){
            //std::cout << "Full 2 Size: " << std::to_string(m_size) << std::endl;
            return;
        }

        buffer[head_] = std::move(value);
        head_ = (head_ + 1 >= max_size_ ? 0 : head_+1);
        if(head_ == tail_)
            tail_ = (tail_ + 1 >= max_size_ ? 0 : tail_+1);
        m_size++;
    }

    /*
    *
    */
    const T& get(){
        //std::cout << "CircularBuffer " + std::string(__func__) + " Get started" << " Size: " << std::to_string(m_size)<< std::endl;
        std::lock_guard<std::mutex> lk(cv_m);

        const int ptail = tail_;
        if(!is_empty()){
            tail_ = (tail_ + 1 >= max_size_ ? 0 : tail_+1);
        }
        m_size--;

        const T& item = std::move(buffer[ptail]);
        return item;
    }

    /*
    * Empty buffer
    */
    const void empty(){
        std::lock_guard<std::mutex> lk(cv_m);
        //TODO: Delete elements
        tail_ = head_;
    }

    /*
    * Check if buffer is empty
    */
    const bool is_empty() const{
        return (m_size == 0);
    }

private:
    T* buffer;
    int m_size;

    //T m_last_value;
    int tail_ = 0;
    int head_ = 0;
    int max_size_;

    std::mutex cv_m;
};

}
}
 #endif
