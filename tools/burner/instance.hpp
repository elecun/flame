/**
 * @file instance.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Burner global instance
 * @version 0.1
 * @date 2024-04-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_BURNER_INSTANCE_INCLUDED
#define FLAME_BURNER_INSTANCE_INCLUDED

#include <dep/libzmq/zmq.h>
#include <thread>
#include <vector>

#include "cli.hpp"


using namespace std;

namespace flame_rt {

    static remote_cli* _cli = nullptr;

    /**
     * @brief initialization
     * 
     * @param config_path configuration file(*.conf) path(absolute)
     * @return true if success
     * @return false if failed
     */
    bool init(const char* config_path);

    /**
     * @brief run application with bundles
     * 
     */
    void run();

    void cleanup();
    void signal_callback(int sig);
    void terminate();

    /* database */
    void db_init();
    bool db_open();
} /* namespace */

#endif