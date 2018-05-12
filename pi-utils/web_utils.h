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
            page.assign( (std::istreambuf_iterator<char>(istrm) ),
                            (std::istreambuf_iterator<char>()    ) );
        }

        return page;
    }

    //
    //Replace field by value
    //
    static const PageContent replace_value(const PageContent page, const std::string& field, const std::string& value){
        
        std::string::size_type pos = page.find(field);
        if(pos != std::string::npos){
            PageContent new_page(std::move(page));
            return new_page.replace(pos, field.size(), value);
        }
        return page;        
    }

};


}//namespace webutils
}//namespace piutils

#endif