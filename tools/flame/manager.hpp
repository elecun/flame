/**
 * @file manager.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Flame Service Manager
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_MANAGER_HPP_INCLUDED
#define FLAME_MANAGER_HPP_INCLUDED

#include <vector>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <flame/component/driver.hpp>
#include <flame/arch/singleton.hpp>
#include <flame/util/uuid.hpp>
#include <zmq.hpp>
#include <tuple>

using namespace std;
namespace fs = std::filesystem;

namespace flame {

    class bundle_manager : public flame::arch::singleton<bundle_manager>{
        public:
            typedef map<util::uuid_t, flame::component::driver*> bundle_container_t;
            typedef map<string, zmq::context_t*> dataport_ctx_map_t; //component name, data port context (one component has one port context)
            typedef map<string, zmq::context_t*> serviceport_ctx_map_t; //component name, service port context

            bundle_manager();
            virtual ~bundle_manager();

            // components installation in bundle repository
            bool install(fs::path repository);
            void uninstall(const char* component_name = nullptr); //install without system configuration

            void start_bundle_service();

            // flame::component::driver* get_driver(const char* component_name = nullptr);
            // flame::component::runnable* get_component(const char* component_name = nullptr);

        public:
            

        private:
            bundle_container_t _bundle_container;
            unordered_map<string, util::uuid_t> _component_uid_map;
            util::uuid_generator _uuid_gen;

            // bundle data port context (only inproc)
            dataport_ctx_map_t _dp_ctx_map;
            serviceport_ctx_map_t _sp_ctx_map;

    }; /* class */

} /* namespace */

#define manager flame::bundle_manager::instance()

#endif