/*
* Service functions used for internal Web
*
* Created by Denis Kudia
*
*/
#ifndef _PIUTILS_WEB_UTILS
#define _PIUTILS_WEB_UTILS

#include <fstream>
#include <string>
#include <map>

namespace piutils{
namespace webutils{

using PageContent = std::string;

class WebUtils {

public:
    //
    // Load page content
    //
    static const PageContent load_page(const std::string& filename){
        PageContent page;
        std::ifstream istrm(filename, std::ios::binary);

        if(istrm.is_open()){
            page.assign( (std::istreambuf_iterator<char>(istrm) ), (std::istreambuf_iterator<char>()));
        }

        return page;
    }

    //
    //Replace field by value
    //
    static const std::string replace_values(const std::string& src, const std::map<std::string, std::string>& values,
        const std::string& v_start = "<!--{", const std::string& v_end = "}-->"){

        std::string res;

        size_t s_pos = 0;
        size_t pos = src.find(v_start, s_pos);
        if(pos == std::string::npos){
            return src;
        }

        while(pos != std::string::npos){
            size_t e_pos = src.find(v_end, pos);
            if(e_pos == std::string::npos){
                //something wrong
                break;
            }
            size_t p_len = e_pos - pos + v_end.length();
            std::string pattern = src.substr(pos, p_len);
            auto item = values.find(pattern);
            if(item != values.end()){
                res += src.substr(s_pos, pos - s_pos);
                res += item->second;
            }
            else{
                res += src.substr(s_pos, pos - s_pos + p_len);
            }

            s_pos = pos + p_len;
            pos = src.find(v_start, s_pos);
        }

        res += src.substr(s_pos);

        return res;
    }

};


}//namespace webutils
}//namespace piutils

#endif