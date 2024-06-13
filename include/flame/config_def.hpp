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
#define __PROFILE_FILE_EXT__ ".profile"

// environments key properties


//bundle key properties
#if defined(__CONFIG_KEY_BUNDLE__)
    #define __CONFIG_KEY_BUNDLE_NAME__ "name"
#endif

#endif