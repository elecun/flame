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

#include <atomic>

/**
 * @brief Global shutdown request flag
 * 
 */
extern volatile std::atomic<bool> g_shutdown_requested;

/**
 * @brief Initialize the flame instance with configuration
 * 
 * @param config_path Path to the configuration file
 * @return true if initialization is successful
 * @return false if initialization fails
 */
bool init(const char* config_path);

/**
 * @brief Cleanup resources and exit the program
 * 
 */
void cleanup_and_exit();

/**
 * @brief Cleanup resources
 * 
 */
void cleanup();

/**
 * @brief Signal event callback function
 * 
 * @param sig Signal number
 */
void signal_callback(int sig);

/**
 * @brief Install the bundle
 * 
 * @param bundle Bundle name (optional)
 * @return true if installation corresponds to success
 * @return false if installation fails
 */
bool install_bundle(const char* bundle = nullptr);

/**
 * @brief Run the installed bundle
 * 
 */
void run_bundle();



#endif
