/**
 * @file config_def.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Definitions for Configuration Parsing
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_CONFIG_DEF_HPP_INCLUDED
#define FLAME_CONFIG_DEF_HPP_INCLUDED

// root config key
#define __CONFIG_KEY_ENVIRONMENTS__ "environments"
#define __CONFIG_KEY_BUNDLE__ "bundle"
#define __COMPONENT_FILE_EXT__ ".comp"
#define __PROFILE_FILE_EXT__ ".json"

// environments key properties

//profile key
#define __PROFILE_RT_CYCLE_NS__ "rt_cycle_ns"
#define __PROFILE_DATAPORT__ "dataport"

//bundle key properties
#if defined(__CONFIG_KEY_BUNDLE__)
    #define __CONFIG_KEY_BUNDLE_NAME__ "name"
    #define __CONFIG_KEY_BUNDLE_TOPOLOGY__ "topology"
#endif

#include <cxxabi.h>
#include <string>
inline const char* __demangle__(const char* name){
    int status;
    char* demangled = abi::__cxa_demangle(name, 0, 0, &status);
    std::string str(demangled);
    return str.c_str();
}
#define _THIS_COMPONENT_ __demangle__((const char*)typeid(*this).name())

#endif