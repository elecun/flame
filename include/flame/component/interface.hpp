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
#include <flame/component/type.hpp>

namespace flame::component {
    class Driver;
    class Interface {
        friend class flame::component::Driver;
        
        virtual bool onInit() = 0;                     //initialize : entry
        virtual void onClose() = 0;                    //destroy
        virtual void onLoop() = 0;                     //cycle
        virtual void onData(ZData& data) = 0;  // data event (multipart_t - moved from socket receiver)
    }; /* class */

} /* namespace */

#endif