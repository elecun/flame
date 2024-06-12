/**
 * @file component.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component Interface Class
 * @version 0.1
 * @date 2024-06-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_COMPONENT_HPP_INCLUDED
#define FLAME_COMPONENT_HPP_INCLUDED

#include <string>
#include <map>

using namespace std;

namespace flame { class driver; }
namespace flame {
    class component {
        friend class flame::driver;

        public:
            component() = default;
            virtual ~component() = default;

            virtual void on_init() = 0;
            virtual void on_loop() = 0;
            virtual void on_close() = 0;
            virtual void on_message() = 0;

            virtual const char* get_name() { return _name.c_str(); }

        protected:

        private:
            string _name = {"noname"};

    }; /* class */

    typedef flame::component*(*create_component)(void);
    typedef void(*release_component)(void);

    #define EXPORT_COMPONENT_API extern "C" { flame::component* create_component(void); void release(void); }

} /* namespace */

#endif