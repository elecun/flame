/**
 * @file driver.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Component Driver with core interface
 * @version 0.1
 * @date 2023-07-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef FLAME_DRIVER_HPP_INCLUDED
#define FLAME_DRIVER_HPP_INCLUDED

#include <filesystem>
#include <csignal>
#include <flame/core/component.hpp>

using namespace std;

namespace flame {

    class driver {
        public:
            driver(const char* component_name);
            driver(flame::component* instance);
            driver(const fs::path component);

            void on_init();
            void on_loop();
            void on_close();

            // driver service function
            const char* get_name() {
                if(_componentImpl)
                    return _componentImpl->get_name();
                return nullptr;
            }

        private:
            bool load(const char* component_name);
            void unload();

        private:
            flame::component* _componentImpl { nullptr };
            void* _component_handle { nullptr };
            struct sigevent _signal_event;
            struct itimerspec _time_spec;

    }; /* class*/

} /* namespace */

#endif
