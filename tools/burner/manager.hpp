/**
 * @file manager.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Bundle Manager
 * @version 0.1
 * @date 2024-05-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_RT_BURNER_MANAGER_HPP_INCLUDED
#define FLAME_RT_BURNER_MANAGER_HPP_INCLUDED

#include <flame/extra/singleton.hpp>


namespace flame_rt::burner {
    class bundle_manager : public flame_rt::extra::singleton<bundle_manager> {
        public:
        bundle_manager();
        virtual ~bundle_manager();

        bool load(const char* bundle_name);
        void unload();


    }; /* class */
} /* namespace */

#define bundle_manager flame_rt::burner::instance()

#endif