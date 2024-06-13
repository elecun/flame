/**
 * @file interface.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component Inteface
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_COMPONENT_INTERFACE_HPP_INCLUDED
#define FLAME_COMPONENT_INTERFACE_HPP_INCLUDED

namespace flame::component {
    class interface {
        virtual void on_init() = 0;
        virtual void on_loop() = 0;
        virtual void on_close() = 0;
        virtual void on_message() = 0;
    }; /* class */
} /* namespace */

#endif