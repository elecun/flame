/**
 * @file driver.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component Driver
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_COMPONENT_DRIVER_HPP_INCLUDED
#define FLAME_COMPONENT_DRIVER_HPP_INCLUDED

#include <flame/component/object.hpp>
#include <flame/component/interface.hpp>
#include <filesystem>
#include <memory>
#include <csignal>
#include <chrono>
#include <type_traits>

using namespace std;
namespace fs = std::filesystem;

namespace flame::component {
    class driver : public component::interface {
        public:
            driver(fs::path component_path); //without extension
            virtual ~driver();

            bool on_init() override;
            void on_loop() override;
            void on_close() override;
            void on_message() override;

            const char* get_name() {
                if(_componentImpl)
                    return _componentImpl->get_name();
                return nullptr;
            }

        private:
            /**
             * @brief component file(*.comp) load
             * 
             * @param component_path 
             * @return true if success
             * @return false if failed
             */
            bool load(fs::path component_path);

            /**
             * @brief unload the component file(*.comp)
             * 
             */
            void unload();

        private:
            flame::component::object* _componentImpl { nullptr };
            void* _component_handle { nullptr };
            struct sigevent _signal_event;
            struct itimerspec _time_spec;


    }; /* class */
} /* namespace */

#endif