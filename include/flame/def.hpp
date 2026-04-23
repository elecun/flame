/**
 * @file def.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Definitions for Configuration Parsing
 * @version 0.1
 * @date 2024-06-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef FLAME_DEF_HPP_INCLUDED
#define FLAME_DEF_HPP_INCLUDED

#include <string>

namespace flame::def {
// root config key
constexpr const char *kEnvironments = "environments";
constexpr const char *kBundle = "bundle";
constexpr const char *kComponentExt = ".comp";
constexpr const char *kProfileExt = ".json";
constexpr const char *kBundleConfigExt = ".conf";

// profile key
constexpr const char *kProfileRtCycleNs = "rt_cycle_ns";
constexpr const char *kProfileDataport = "dataport";
constexpr const char *kProfileParameters = "parameters";

// bundle key properties
constexpr const char *kBundleName = "name";
constexpr const char *kBundleTopology = "topology";

// monitor
constexpr const char *kFlameConfMonitor = "monitor";
constexpr const char *kFlameConfMonitorAddr = "address";
constexpr const char *kFlameMonitorIpcAddrDefault = "ipc:///tmp/flame.ipc";

} // namespace flame::def

#endif