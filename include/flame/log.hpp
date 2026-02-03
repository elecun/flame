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
#include <dep/spdlog/sinks/stdout_color_sinks.h>
#include <dep/spdlog/spdlog.h>
#include <iostream>
namespace logger = spdlog;

#include <string>
#include <vector>
using namespace std;
inline int str2level(string &str) {
  if (!str.compare("trace")) {
    return logger::level::trace;
  } else if (!str.compare("debug")) {
    return logger::level::debug;
  } else if (!str.compare("info")) {
    return logger::level::info;
  } else if (!str.compare("warn")) {
    return logger::level::warn;
  } else if (!str.compare("err")) {
    return logger::level::err;
  } else if (!str.compare("critical")) {
    return logger::level::critical;
  } else if (!str.compare("off")) {
    return logger::level::off;
  }

  return -1;
}

inline void create(string verbose_level = "info") {
  int _verbose_level_i = str2level(verbose_level);

  /* default logger sink */
  auto console_sink = std::make_shared<logger::sinks::stdout_color_sink_mt>();
  console_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
  std::vector<logger::sink_ptr> sinks{console_sink};

  /* console logger sink (quiet) */
  auto quiet_sink = std::make_shared<logger::sinks::stdout_color_sink_mt>();
  quiet_sink->set_pattern("[%^%l%$] %v");
  std::vector<logger::sink_ptr> console_sinks{quiet_sink};

  /* set logger set */
  auto logger =
      std::make_shared<logger::logger>("flame", sinks.begin(), sinks.end());
  logger::set_default_logger(logger);
  logger::set_level(static_cast<logger::level::level_enum>(_verbose_level_i));

  /* set console logger */
  auto console_logger = std::make_shared<logger::logger>(
      "console", console_sinks.begin(), console_sinks.end());
  console_logger->set_level(
      static_cast<logger::level::level_enum>(_verbose_level_i));
  logger::register_logger(console_logger);
}

namespace spdlog {
template <typename... Args>
inline void log(format_string_t<Args...> fmt, Args &&...args) {
  std::cout << fmt::format(fmt, std::forward<Args>(args)...) << std::endl;
}
} // namespace spdlog

#endif

#endif