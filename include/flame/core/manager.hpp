/**
 * @file manager.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Manager classes for component
 * @version 0.1
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _FLAME_CORE_MANAGER_HPP_
#define _FLAME_CORE_MANAGER_HPP_

#include <flame/arch/singleton.hpp>
#include <flame/util/uuid.hpp>
#include <flame/core/driver.hpp>
#include <openedge/core/driver.hpp>
#include <openedge/core/task.hpp>
#include <openedge/core/def.hpp>
#include <string>
#include <map>

using namespace std;

namespace oe::core {

    class task_manager : public oe::arch::singleton<task_manager> {
        public:
            typedef map<util::uuid_t, oe::core::task::driver*> task_container_t;

            task_manager();
            virtual ~task_manager();

            /**
             * @brief install the task by name or concrete instance
             * 
             * @param taskname task name string
             * @param instannce concrete instance
             * @return true if sucessfully installed
             * @return false if failed
             */
            bool install(const char* taskname, oe::core::task::runnable* instance = nullptr);

            void uninstall(const char* taskname = nullptr); //install without system configuration

            void run(const char* taskname = nullptr);
            void cleanup(const char* taskname = nullptr);
            bool exist(const char* taskname);
            util::uuid_t find_uuid(const char* taskname);

            int size() const { return _task_container.size(); }

            oe::core::task::driver* get_driver(const char* taskname = nullptr);
            oe::core::task::runnable* get_task(const char* taskname = nullptr);

        protected:
            //task_manager(const task_manager&) = delete;
            task_manager& operator = (const task_manager&) = delete;

        private:
            task_container_t _task_container;
            unordered_map<string, util::uuid_t> _task_uid_map;
            util::uuid_generator _uuid_gen;
    }; //end class
} /* namespace */

#define manager oe::core::task_manager::instance()

#endif