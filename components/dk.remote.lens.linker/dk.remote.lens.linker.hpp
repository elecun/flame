/**
 * @file remote.lens.linker.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Lens device linker working on remote-side controller
 * @version 0.1
 * @date 2024-06-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_DK_REMOTE_LENS_LINKER_HPP_INCLUDED
#define FLAME_DK_REMOTE_LENS_LINKER_HPP_INCLUDED

#include <flame/component/object.hpp>


class dk_remote_lens_linker : public flame::component::object {
    public:
        dk_remote_lens_linker() = default;
        virtual ~dk_remote_lens_linker() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

}; /* class */

EXPORT_COMPONENT_API


#endif