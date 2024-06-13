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

bool init(const char* config_path);
void cleanup_and_exit();
void cleanup();
void signal_callback(int sig);
bool install_bundle(const char* bundle = nullptr);  // install bundle
void run_bundle();   // perform bundle



#endif
