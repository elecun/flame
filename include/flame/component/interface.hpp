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

#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace flame::component {
    class driver;

    class interface {
        friend class flame::component::driver;
        
        virtual bool on_init() = 0;     // initialize
        virtual void on_close() = 0;    //destroy
        virtual void on_loop() = 0;     //cycle
        virtual void on_message() = 0;  //message event
    }; /* class */

} /* namespace */

#endif