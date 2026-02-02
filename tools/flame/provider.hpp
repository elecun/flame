/**
 * @file provider.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief State Provider for Flame Instance
 * @version 0.1
 * @date 2024-07-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_PROVIDER_HPP_INCLUDED
#define FLAME_PROVIDER_HPP_INCLUDED

#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <flame/common/zpipe.hpp>

namespace flame {

    class StateProvider {
    public:
        StateProvider();
        virtual ~StateProvider();

        void start();
        void start_subscribe();
        void stop();

    private:
        void _publish_loop();
        void _subscribe_loop();

    private:
        std::atomic<bool> _run_service;
        std::thread* _t_service;
        std::string _epgm_addr;
    };

} /* namespace */

#endif
