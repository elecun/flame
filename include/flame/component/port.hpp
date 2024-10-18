/**
 * @file port.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Definitions for Data & Service Pipeline
 * @version 0.1
 * @date 2024-07-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_COMPONENT_PORT_HPP_INCLUDED
#define FLAME_COMPONENT_PORT_HPP_INCLUDED

#include <zmq.hpp>
#include <unordered_map>

using namespace std;

namespace flame {
    
    using pipe_context = zmq::context_t;    /* pipeline */
    using pipe_socket = zmq::socket_t;      /* socket */
    using pipe_data = zmq::message_t;       /* message */

    enum class socket_type : int {
        sub = 0,
        pub,
        push,
        pull,
        req,
        rep
    };


    inline socket_type str2type(const std::string& str_type) {
        const std::unordered_map<std::string, socket_type> s_type = {
            {"sub", socket_type::sub},
            {"pub", socket_type::pub},
            {"push", socket_type::push},
            {"pull", socket_type::pull},
            {"req", socket_type::req},
            {"rep", socket_type::rep}
        };

        auto it = s_type.find(str_type);
        if (it != s_type.end()) {
            return it->second;
        } else {
            throw std::invalid_argument("Invalid type string");
        }
    }
}


#endif