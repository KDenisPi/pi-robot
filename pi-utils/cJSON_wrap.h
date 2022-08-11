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
    /*
     Parser
    */
    CJsonWrap(const floader& fl) {
        cjson = cJSON_Parse(fl->get());
        if(cjson == NULL){ //Print parse error error
            perror= std::string(cJSON_GetErrorPtr());
        }
    }

    /*
    Printer
    */
    CJsonWrap() {
        cjson = cJSON_CreateObject();
        if(cjson == NULL){ //Print parse error error
            perror= "Could not create root object";
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
        Detect if we have element with name
    */
   const bool contains(const std::string& name, const cj_obj obj = nullptr) {
        const cj_obj obj_i = cJSON_GetObjectItemCaseSensitive((obj==nullptr ? cjson : obj), name.c_str());
        return (obj_i != nullptr);
   }

    /*
        Get object
    */
    const cj_obj get_object(const std::string& name) noexcept(false) {
        return get_object(cjson, name);
    }

    const cj_obj get_object(const cj_obj obj, const std::string& name) noexcept(false) {
        const cj_obj obj_i = cJSON_GetObjectItemCaseSensitive(obj, name.c_str());
        if(!cJSON_IsObject(obj_i))
            throw std::runtime_error(std::string("Absent object. Name: ") + name);

        return obj_i;
    }

    /*
        Get array
    */
    const cj_obj get_array(const std::string& name) noexcept(false) {
        return get_array(cjson, name);
    }

    const cj_obj get_array(const cj_obj obj, const std::string& name) noexcept(false) {
        const cj_obj obj_i = cJSON_GetObjectItemCaseSensitive(obj, name.c_str());
        if(!cJSON_IsArray(obj_i))
            throw std::runtime_error(std::string("Absent array. Name: ") + name);

        return obj_i;
    }

    const cj_obj get_first(const cj_obj array) const {
        return (cJSON_IsArray(array) ? array->child : nullptr);
    }

    const cj_obj get_next(const cj_obj item) const {
        return (item != nullptr ? item->next : nullptr);
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

    bool add_string(cj_obj obj_i, const std::string& name, const std::string& val) noexcept(false){
        if(!cJSON_IsObject(obj_i))
            throw std::runtime_error(std::string("Root object is absent for name: ") + name);
        if(name.empty())
            throw std::runtime_error(std::string("No name"));

        cj_obj value = cJSON_CreateString(val.c_str());
        if(value == nullptr){
            throw std::runtime_error(std::string("Could not create string Name: ") + name);
        }

        cJSON_AddItemToObject(obj_i, name.c_str(), value);
        return true;
    }

    template<class T>
    bool add_number(cj_obj obj_i, const std::string name, T& val) noexcept(false){
        if(!cJSON_IsObject(obj_i))
            throw std::runtime_error(std::string("Root object is absent for name: ") + name);
        if(name.empty())
            throw std::runtime_error(std::string("No name"));

        cj_obj value = cJSON_CreateNumber(val);
        if(value == nullptr){
            throw std::runtime_error(std::string("Could not create number Name: ") + name);
        }

        cJSON_AddItemToObject(obj_i, name.c_str(), value);
        return true;
    }

    cj_obj add_array(cj_obj obj_i, const std::string name) noexcept(false) {
        if(!cJSON_IsObject(obj_i))
            throw std::runtime_error(std::string("Root object is absent for name: ") + name);
        if(name.empty())
            throw std::runtime_error(std::string("No name"));

        auto a_obj = cJSON_CreateArray();
        if(!cJSON_IsArray(a_obj))
            throw std::runtime_error(std::string("Could not create array Name: ") + name);

        cJSON_AddItemToObject(obj_i, name.c_str(), a_obj);
        return a_obj;
    }

    cj_obj add_object(cj_obj obj_i, const std::string name) noexcept(false) {
        if(!cJSON_IsObject(obj_i))
            throw std::runtime_error(std::string("Root object is absent for name: ") + name);
        if(name.empty())
            throw std::runtime_error(std::string("No name"));

        auto a_obj = cJSON_CreateObject();
        if(!cJSON_IsObject(a_obj))
            throw std::runtime_error(std::string("Could not create object Name: ") + name);

        cJSON_AddItemToObject(obj_i, name.c_str(), a_obj);
        return a_obj;
    }


    const std::string print() const {
        return std::string(cJSON_Print(this->get()));
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
