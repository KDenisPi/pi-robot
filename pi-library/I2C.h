/*
 * I2C.h
 *
 *  Created on: Dec 21, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_I2C_H_
#define PI_LIBRARY_I2C_H_

#include <vector>
#include "provider.h"
#include "I2CWrapper.h"

namespace pirobot {
namespace i2c {

using i2c_user = std::pair<std::string, uint8_t>;

class I2C : public provider::Provider, public I2CWrapper {

public:
    I2C() : provider::Provider(provider::PROVIDER_TYPE::PROV_I2C, "I2C") {}
    virtual ~I2C() {}

    /*
    // Add an Item connected to I2C
    */
    int add_user(const std::string name, const uint8_t addr){
        int fd = I2CSetup(addr);
        m_users.push_back(std::make_pair(name, addr));

        return fd;
    }

    void del_user(const std::string name, const int fd){
        I2CClose(fd);
        //TODO: remove from list
    }

    /*
    // Return list of items connected to I2C
    */
    const std::vector<std::pair<std::string, uint8_t>>& get_users(){
        return m_users;
    }

    virtual const std::string printConfig() override{
        std::string result = "I2C\n";
        for(auto p : m_users){
            result +=  p.first + " " + std::to_string(p.second) + "\n";
        }
        return result;
    }

private:
    std::vector<std::pair<std::string, uint8_t>> m_users;
};

} //i2c
} //pirobot
#endif
