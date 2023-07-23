/**
 * @file file.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief File handling utility
 * @version 0.1
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#ifndef _FLAME_UTIL_FILE_HPP_
#define _FLAME_UTIL_FILE_HPP_

#include <sys/stat.h>
#include <unistd.h>

namespace flame::util {
    /**
     * @brief file existance check
     * 
     * @param filepath file path
     * @return true if exist
     * @return false if not exist
     */
  inline bool exist(const char* filepath) {
    struct stat buffer;   
    return (stat(filepath, &buffer) == 0); 
  }
}

#endif