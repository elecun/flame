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
#include <dep/spdlog/sinks/rotating_file_sink.h>
namespace logger = spdlog;

#include <string>
using namespace std;
inline int str2level(string& str){
    if(!str.compare("trace")) { return logger::level::trace; }
    else if(!str.compare("debug")) { return logger::level::debug; }
    else if(!str.compare("info")) { return logger::level::info; }
    else if(!str.compare("warn")) { return logger::level::warn; }
    else if(!str.compare("err")) { return logger::level::err; }
    else if(!str.compare("critical")) { return logger::level::critical; }
    else if(!str.compare("off")) { return logger::level::off; }
    
    return -1;
}


#endif


#endif