/**
 * @file object.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component Object (*.comp)
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef FLAME_COMPONENT_OBJECT_HPP_INCLUDED
#define FLAME_COMPONENT_OBJECT_HPP_INCLUDED

#include <filesystem>
#include <flame/component/interface.hpp>

using namespace std;
namespace fs = std::filesystem;

namespace flame::component {
    class driver;
    class object : public component::interface {
        friend class flame::component::driver;

        public:
            object() = default;
            virtual ~object() = default;

            // no interface override

            virtual const char* get_name() { return _name.c_str(); }

        protected:

        private:
            string _name = {"noname"};
            
    }; /* class */

    typedef flame::component::object*(*create_component)(void);
    typedef void(*release_component)(void);

    #define EXPORT_COMPONENT_API extern "C" { flame::component::object* create_component(void); void release(void); }

} /* namespace */

#endif