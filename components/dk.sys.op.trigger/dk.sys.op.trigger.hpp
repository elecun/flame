/**
 * @file dk.sys.op.trigger.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief 
 * @version 0.1
 * @date 2024-06-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_DK_SYS_OP_TRIGGER_HPP_INCLUDED
#define FLAME_DK_SYS_OP_TRIGGER_HPP_INCLUDED

#include <flame/component/object.hpp>


class dk_sys_op_trigger : public flame::component::object {
    public:
        dk_sys_op_trigger() = default;
        virtual ~dk_sys_op_trigger() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

}; /* class */

EXPORT_COMPONENT_API


#endif