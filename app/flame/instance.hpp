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

#ifndef _FLAME_INSTANCE_HPP_
#define _FLAME_INSTANCE_HPP_

namespace flame::app {

    /**
     * @brief load configration file to prepare for running
     * 
     * @param conf_file_path configuration file path
     * @return true if success
     * @return false if failed
     */
    bool initialize(const char* conf_file_path);

    /**
     * @brief start to run all components
     * 
     */
    void run();

    /**
     * @brief cleanup the all process
     * 
     */
    void cleanup();
}

#endif
