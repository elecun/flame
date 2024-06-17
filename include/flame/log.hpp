/**
 * @file log.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Logger interface
 * @version 0.1
 * @date 2024-04-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_LOG_HPP_INCLUDED
#define FLAME_LOG_HPP_INCLUDED

/* pre-defined & editable */
#define _USE_SPDLOG_

#ifdef _USE_SPDLOG_
#include <dep/spdlog/spdlog.h>
#include <dep/spdlog/sinks/stdout_color_sinks.h>
#include <dep/spdlog/sinks/basic_file_sink.h>
namespace console = spdlog;
#endif

#include <cxxabi.h>
inline const char* __demangle__(const char* name){
    int status;
    char* demangled = abi::__cxa_demangle(name, 0, 0, &status);
    std::string str(demangled);
    return str.c_str();
}
#define _THIS_COMPONENT_ __demangle__((const char*)typeid(*this).name())


#endif