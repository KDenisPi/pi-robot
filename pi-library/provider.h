/*
 * Provider.h
 *
 *  Created on: Aug 19, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_LIBRARY_PROVIDER_H_
#define PI_LIBRARY_PROVIDER_H_

#include <string>

namespace pirobot{
namespace provider{

enum PROVIDER_TYPE {
	PROV_GPIO = 0,
	PROV_SPI  = 1
};

/*
* Provider information.
* Temporary will be used only two parameter: type and name 
* but list of parameters will be extended later
*/
struct Provider_Info{
    PROVIDER_TYPE type;
    std::string name;
};

class Provider {
public:
    Provider(const PROVIDER_TYPE type, const std::string name) :
        m_type(type), m_name(name) {

        }

    virtual ~Provider(){

    }

    const std::string get_name() const {return m_name;}
    const PROVIDER_TYPE get_ptype() const {return m_type;}
    
private:
    PROVIDER_TYPE m_type;
    std::string m_name;
};

}//end namespace provider
}//end namespace pirobot
#endif /* PI_LIBRARY_PROVIDER_H_ */

