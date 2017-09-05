/*
 * Motor.h
 * Some common functionality for different types of motors
 * 
 *  Created on: Aug 31, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_MOTOR_H_
#define PI_LIBRARY_MOTOR_H_

#include <mutex>

namespace pirobot {
namespace item {

class Motor{
public:
    Motor() : m_rotate(true) {}
    virtual ~Motor() {}

    const bool is_rotate() const{
        return m_rotate;
    }

    void stop_rotation(){
        if(!m_rotate)
            return;

        std::lock_guard<std::mutex> lk(cv_motor);
        m_rotate = false;
    }

    void set_rotation(const bool rotation){
        m_rotate = false;
    }
    
private:
    std::mutex cv_motor;	
    bool m_rotate;

}; 
        
}
}

#endif
 