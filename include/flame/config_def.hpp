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

#include <string>

namespace flame::config::def {
    // root config key
    constexpr const char* ENVIRONMENTS = "environments";
    constexpr const char* BUNDLE = "bundle";
    constexpr const char* COMPONENT_EXT = ".comp";
    constexpr const char* PROFILE_EXT = ".json";
    constexpr const char* BUNDLE_CONFIG_EXT = ".conf";

    // profile key
    constexpr const char* PROFILE_RT_CYCLE_NS = "rt_cycle_ns";
    constexpr const char* PROFILE_DATAPORT = "dataport";
    constexpr const char* PROFILE_PARAMETERS = "parameters";

    // bundle key properties
    constexpr const char* BUNDLE_NAME = "name";
    constexpr const char* BUNDLE_TOPOLOGY = "topology";
}

#endif