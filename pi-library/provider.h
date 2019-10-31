/*
 * Provider.h
 *
 *  Created on: Aug 19, 2017
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_PROVIDER_H_
#define PI_LIBRARY_PROVIDER_H_

#include <string>
#include <functional>
#include <memory>

namespace pirobot{
namespace provider{

enum PROVIDER_TYPE {
    PROV_GPIO = 0,
    PROV_SPI  = 1,
    PROV_I2C = 2,
    PROV_PWM = 3
};

class ProviderData {
public:

    ProviderData() noexcept {}
    ProviderData(const int pin, const int value) noexcept : _i_pin(pin), _i_value(value) {}

    int _i_pin;
    int _i_value;
};

class  ProviderInfo {
public:

    ProviderInfo(const std::string name, const std::string comment = "") :
        m_name(name), m_comment(comment) {}

    virtual ~ProviderInfo() {}

    const std::string get_name() const {
        return m_name;
    }

    const std::string comment() const {
        return m_comment;
    }

protected:
    std::string m_comment;
    std::string m_name;
};

class Provider : public ProviderInfo{
public:
    Provider(const PROVIDER_TYPE type, const std::string name, const std::string comment = "") :
        ProviderInfo(name, comment), m_type(type) {

        }

    virtual ~Provider(){ }

    const PROVIDER_TYPE get_ptype() const {
        return m_type;
    }

    virtual const std::string printConfig() = 0;

    std::function<void(const pirobot::provider::PROVIDER_TYPE, const std::string&, std::shared_ptr<ProviderData>)> notify;

private:
    PROVIDER_TYPE m_type;
};

}//end namespace provider
}//end namespace pirobot
#endif /* PI_LIBRARY_PROVIDER_H_ */

