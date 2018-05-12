/*
* Receiving network interfaces information
*
* Created by Denis Kudia
* 
*/
#ifndef _PIUTILS_NETWORKINFO
#define _PIUTILS_NETWORKINFO

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <list>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>

namespace piutils {
namespace netinfo {

#define NIPQUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

#define NIPQUAD_FMT "%u.%u.%u.%u"

#define NIP6(addr) \
        ntohs((addr).s6_addr16[0]), \
        ntohs((addr).s6_addr16[1]), \
        ntohs((addr).s6_addr16[2]), \
        ntohs((addr).s6_addr16[3]), \
        ntohs((addr).s6_addr16[4]), \
        ntohs((addr).s6_addr16[5]), \
        ntohs((addr).s6_addr16[6]), \
        ntohs((addr).s6_addr16[7])

#define NIP6_FMT "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"

enum IpType {
    IP_V4,
    IP_V6
};

template<class T>
class IpAddress {
public:

    IpAddress(const T* ip_data, std::string label = ""){
        memcpy((void*)&_ip_data, ip_data, sizeof(T));
        _iptype = (4==sizeof(T) ? IP_V4 : IP_V6);
        _label = std::move(label);
    }

    virtual ~IpAddress() {}

    const std::string& get_label() const {
        return _label;
    }

    const bool is_ip4() const {
        return (_iptype == IP_V4);
    }

    const T* get_data() const {
        return &_ip_data;
    }

    const std::string to_string() {
        return to_string(_ip_data);
    }

private:
    IpType _iptype;
    std::string _label;
    T _ip_data;

    const std::string to_string(const struct in_addr& data){
        char buffer[50];
        std::sprintf(buffer, NIPQUAD_FMT, NIPQUAD(data));
        std::string result(buffer);

        return result;
    }

    const std::string to_string(const struct in6_addr& data){
        char buffer[50];
        std::sprintf(buffer, NIP6_FMT, NIP6(data));
        std::string result(buffer);

        return result;
    }
};

using IpAddress_V6 = IpAddress<struct in6_addr>;
using IpAddress_V4 = IpAddress<struct in_addr>;

using ItfInfo = std::pair<std::string, std::string>;

/*
* Network interface information
*/
class NetInterface {
public:
    NetInterface(const int index) : _index(index) {}
    virtual ~NetInterface() {}

    //
    const int get_index() const {
        return _index;
    }
    //
    const std::string& get_label() const {
        return _label;
    }
    //
    void set_label(std::string label){
        _label = std::move(label);
    }
    //
    void set_label(char* label){
        _label = label;
    }
    //
    void add_ip_v4(std::shared_ptr<IpAddress_V4> ip4){
        _ip4.push_back(ip4);
    }
    //
    void add_ip_v6(std::shared_ptr<IpAddress_V6> ip6){
        _ip6.push_back(ip6);
    }

    /*

    */
    const std::string get_ip(IpType ip_type, const std::string label = "address 0") {
        std::string result;
        if(ip_type == IpType::IP_V4){
            for(auto ip4 : _ip4){
                if(label == ip4->get_label()){
                    result = ip4->to_string();
                    break;
                }
            }
        }
        else{
            for(auto ip6 : _ip6){
                if(label == ip6->get_label()){
                    result = ip6->to_string();
                    break;
                }
            }
        }

        return result;
    }

    // To string
    const std::string to_string(){
        std::string result = "Interface: Index: " + std::to_string(_index) + " Label: " + _label + "\n";

        for(auto ip4 : _ip4){
            result += ip4->get_label() + ": " + ip4->to_string() + "\n";
        }
        for(auto ip6 : _ip6){
            result += ip6->get_label() + ": " + ip6->to_string() + "\n";
        }

        return result + "\n";
    }

private:
    std::vector<std::shared_ptr<IpAddress_V6>> _ip6;
    std::vector<std::shared_ptr<IpAddress_V4>> _ip4;

    int _index; //interface index
    std::string _label; //Interface label
};

/*
* List of network interfaces
*/
class NetInfo {
public:
    NetInfo() {}
    virtual ~NetInfo() {}

    /*

    */
    std::shared_ptr<NetInterface>& get_itf(const int index){
        if(_itf.find(index) == _itf.end()){
            _itf.insert( std::pair<int, std::shared_ptr<NetInterface>>(index, std::shared_ptr<NetInterface>(new NetInterface(index))));
        }

        return _itf[index];
    }

    /*

    */
    const std::list<ItfInfo> get_default_ip(const IpType ip_type, const std::string label = "") {
        std::list<ItfInfo> result;
        for(auto it = _itf.begin(); it != _itf.end(); ++it){
            auto itf_label = it->second->get_label();
            if(!label.empty())
            {
                std::size_t found = itf_label.find(label);
                if(found == std::string::npos)
                    continue;
            }

            result.push_back(std::make_pair(itf_label, it->second->get_ip(ip_type)));
        }
        return result;
    }

    /*

    */
    const std::string to_string(){
        std::string result;
        for(auto it = _itf.begin(); it != _itf.end(); ++it){
            result += it->second->to_string();
        }

        return result;
    }

    /*

    */
    void load_ip_list(){
        get_ip_list(true, this);
        get_ip_list(false, this);
    }

    private:
        std::map<int, std::shared_ptr<NetInterface>> _itf;
        static void get_ip_list(bool is_ip4, NetInfo* net_info);
};


}//namespace netinfo
}//namespace piutils

#endif //_PIUTILS_NETWORKINFO