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
#include <filesystem>
#include <tuple>
#include <flame/component/driver.hpp>
#include <flame/arch/singleton.hpp>
#include <flame/util/uuid.hpp>
#include <flame/component/port.hpp>

using namespace std;
namespace fs = std::filesystem;

namespace flame {

    class bundle_manager : public flame::arch::singleton<bundle_manager>{
        public:
            typedef map<util::uuid_t, flame::component::driver*> bundle_container_t;

            bundle_manager();
            virtual ~bundle_manager();

            bool install(fs::path bundle_repo); //install the all components in bundle repository
            void uninstall(const char* component_name = nullptr); // uninstall the all components or specific component
            void start_bundle_service();
            

        private:
            bundle_container_t _bundle_container;
            unordered_map<string, util::uuid_t> _component_uid_map;
            util::uuid_generator _uuid_gen;
            pipe_context* _inproc_context { nullptr };
            

    }; /* class */

} /* namespace */

#define manager flame::bundle_manager::instance()

#endif