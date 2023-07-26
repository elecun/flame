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
     * @brief 
     * 
     * @param conf_file 
     * @return true 
     * @return false 
     */
    bool initialize(const char* conf_file);

    /**
     * @brief 
     * 
     */
    void run();

    /**
     * @brief 
     * 
     */
    void cleanup();
}

#endif
