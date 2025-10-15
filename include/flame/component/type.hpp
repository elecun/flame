/**
 * @file type.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Standard types for component model
 * @version 0.1
 * @date 2025-10-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef FLAME_COMPONENT_TYPE_HPP_INCLUDED
#define FLAME_COMPONENT_TYPE_HPP_INCLUDED

#include <zmq_addon.hpp>

namespace flame::component {

    /* standard message type using zmq::multipart_t type */
    using message_t = zmq::multipart_t;

}

#endif