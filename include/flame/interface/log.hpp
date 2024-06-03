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

#ifndef FLAME_RT_INTERFACE_LOG_HPP_INCLUDED
#define FLAME_RT_INTERFACE_LOG_HPP_INCLUDED

/* pre-defined & editable */
#define _USE_SPDLOG_

#ifdef _USE_SPDLOG_
#include <dep/spdlog/spdlog.h>
#include <dep/spdlog/sinks/stdout_color_sinks.h>
#include <dep/spdlog/sinks/basic_file_sink.h>
namespace console = spdlog;
#endif


#endif