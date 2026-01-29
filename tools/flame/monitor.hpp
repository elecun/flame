/**
 * @file monitor.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Flame Monitor Service
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_MONITOR_HPP_INCLUDED
#define FLAME_MONITOR_HPP_INCLUDED

#include <thread>
#include <atomic>
#include <string>

namespace flame {

    class monitor_service {
        public:
            monitor_service();
            virtual ~monitor_service();

            void start_service();
            void stop_service();

        private:
            void _service_loop();

            std::thread* _t_service;
            std::atomic<bool> _run_service;
            std::string _ipc_addr;

    }; /* class */
} /* namespace */

#endif
