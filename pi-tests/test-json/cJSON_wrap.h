/*
 * cJSON_wrap.h
 *
 * Service class for loading file in memory
 *
 * Created on: Jul 31, 2022
 *      Author: Denis Kudia
 */
#include <memory>
#include <fcntl.h>
#include <unistd.h>

#ifndef CJSON_WRAP_H
#define CJSON_WRAP_H

#include "cJSON.h"

#include "file_loader.h"

namespace piutils {
namespace cjson_wrap {

using floader = std::unique_ptr<piutils::floader::Floader>;
using cj_obj = cJSON*;

class CJsonWrap 
{
public:
    CJsonWrap(const floader& fl) {
        cjson = cJSON_Parse(fl->get());
        if(cjson == NULL){ //Print parse error error
            perror= std::string(cJSON_GetErrorPtr());
        }


    }

    virtual ~CJsonWrap() {
        if(cjson != NULL){
            cJSON_Delete(cjson);
            cjson = NULL;
        }
    }

    const std::string get_error() const {
        return perror;
    }

    const bool is_succeess() const {
        return perror.empty();
    }

    const cj_obj get() const {
        return cjson;
    }

    /*
        Get string value by name and use default value if absent
    */
    const std::string get_attr_string_def(const cj_obj obj, const std::string& name, const std::string& def_val){
        std::string val;
        if(!get_attr_string(obj, name, val)) 
            return def_val;
        return val;
    }

    /*
        Get string value by name and throw exception if absent
    */
    const std::string get_attr_string(const cj_obj obj, const std::string& name) noexcept(false) {
        std::string val;
        if(!get_attr_string(obj, name, val)){
            throw std::runtime_error(std::string("Absent mandatory attribute. Name: ") + name);
        }
        return val;
    }

    /*
        Get array
    */
   const cj_obj get_array(const cj_obj obj, const std::string& name) noexcept(false) {
        const cj_obj obj_i = cJSON_GetObjectItemCaseSensitive(obj, name.c_str());
        if(!cJSON_IsArray(obj_i))
            throw std::runtime_error(std::string("Absent array. Name: ") + name);

        return obj_i;
   }

    /*
        Get value by name and use default value if absent
    */
    template<class T>
    const T get_attr_def(const cj_obj obj, const std::string& name, const T& def_val){
        T val;
        if(!get_attr(obj, name, val)){}
            return def_val;
        return val;
    }

    /*
        Get value by name and throw exceptio if absent
    */
    template<class T>
    const T get_attr(const cj_obj obj, const std::string& name) noexcept(false) {
        T val;
        if(!get_attr(obj, name, val)){
            throw std::runtime_error(std::string("Absent mandatory attribute. Name: ") + name);
        }
        return val;
    }


private:
    /*
        Get string value by name
    */
    const bool get_attr_string(const cj_obj obj, const std::string& name, std::string& val){
        const cj_obj obj_i = cJSON_GetObjectItemCaseSensitive(obj, name.c_str());
        if (cJSON_IsString(obj_i) && (obj_i->valuestring != NULL))
        {
            val = std::string(obj_i->valuestring);
            return true;
        }
        return false;
    }

    /*
        Get numeric and bool value by name
    */
    template<class T>
    const bool get_attr(const cj_obj obj, const std::string& name, T& val){
        const cj_obj obj_i = cJSON_GetObjectItemCaseSensitive(obj, name.c_str());
        //std::cout << name << " " << obj_i->valuedouble << " Int: " << obj_i->valueint << " size: " << sizeof(val) << std::endl;

        if( cJSON_IsBool(obj_i) && sizeof(val)==1)
        {
            val = (T)obj_i->valueint;
            return true;
        }

        if(cJSON_IsNumber(obj_i) && sizeof(val)>1)
        {
            val = (T)obj_i->valuedouble;
            return true;
        }

        return false;
    }



private:
    cj_obj cjson;
    std::string perror;
    
};



}//namespace cjson_wrap
}//namespace piutils

#endif
