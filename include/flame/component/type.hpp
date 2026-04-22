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

#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace flame::component {

    using zcontext = zmq::context_t;

    /* Standard data carrier: zmq::multipart_t
     * Use addstr() for string/json frames, addmem() for binary frames.
     * Frame layout: [src_port, dst_port, payload_type, payload]
     */
    using zdata = zmq::multipart_t;

    enum class payload_type { string, json, binary, number };

}

#endif