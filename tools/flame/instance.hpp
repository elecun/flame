/**
 * @file instance.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Flame Execution engine global instance
 * @version 0.1
 * @date 2023-07-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef FLAME_INSTANCE_HPP_INCLUDED
#define FLAME_INSTANCE_HPP_INCLUDED

#include <dep/libzmq/zmq.hpp>

namespace flame::tools {

    bool init(const char* config_path);
    void cleanup_and_exit();
    void cleanup();
    void signal_callback(int sig);

    void run_bundle();


} /* namespace */

#endif
