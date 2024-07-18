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
#include <flame/component/profile.hpp>
#include <memory>

using namespace std;
using path = std::filesystem::path;

namespace flame::component {
    enum class dtype_status : int { STOPPED=0, WORKING};

    class driver;
    class object  : public interface {
        friend class flame::component::driver;

        public:
            object() = default;
            virtual ~object() = default;

            const char* get_name() const { return _name.c_str(); }
            const dtype_status get_status() const { return _status; }

        protected:
            const component::profile* get_profile() const { 
                return _profile.get();
            }

            zmq::socket_t* get_dataport() const {
                return _dataport;
            }

            zmq::socket_t* get_dataport(string portname) const {
                return _dataport;
            }

        private:
            void set_status(dtype_status s) { _status = s; }

        private:
            dtype_status _status { dtype_status::STOPPED };
            string _name = {"noname"};
            unique_ptr<profile> _profile;

            zmq::socket_t* _dataport { nullptr };
            
            
    }; /* class */

    typedef flame::component::object*(*create_component)(void);
    typedef void(*release_component)(void);

    #define EXPORT_COMPONENT_API extern "C" { flame::component::object* create(void); void release(void); }

} /* namespace */

#endif