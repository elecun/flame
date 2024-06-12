/**
 * @file driver.cc
 * @author Byunghun Hwang
 * @brief Component Driver
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "driver.hpp"
#include <csignal>
#include <dlfcn.h>
#include <chrono>
#include <stdexcept>
#include <filesystem> //to use c++17 filesystem

namespace fs = std::filesystem;
using namespace std;
using namespace flame::core;


#if defined(linux) || defined(__linux) || defined(__linux__)
    static const int SIG_RUNTIME_TRIGGER = (SIGRTMIN);     //signal #64 : Runtime Error
#endif

namespace flame {

    driver::driver(const char* component_name){

    }
    driver::driver(flame::component* instance){

    }

    driver::driver(const fs::path component){

    }

} /* namespace */
