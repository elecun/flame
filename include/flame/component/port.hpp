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
#include <zmq_addon.hpp>
#include <unordered_map>

using namespace std;

namespace flame {
    
    using pipe_context = zmq::context_t;    /* pipeline */
    using pipe_socket = zmq::socket_t;      /* socket */
    using pipe_data = zmq::message_t;       /* message */
    using pipe_data_multipart = zmq::multipart_t; /* multipart message */

    enum class socket_type : int {
        pair = 0,
        pub,
        sub,
        req,
        rep,
        dealer,
        router,
        pull,
        push,
        xpub,
        xsub,
        stream
    };


    inline socket_type str2type(const std::string& str_type) {
        const std::unordered_map<std::string, socket_type> s_type = {
            {"pair", flame::socket_type::pair},
            {"pub", flame::socket_type::pub},
            {"sub", flame::socket_type::sub},
            {"req", flame::socket_type::req},
            {"rep", flame::socket_type::rep},
            {"dealer", flame::socket_type::dealer},
            {"router", flame::socket_type::router},
            {"pull", flame::socket_type::pull},
            {"push", flame::socket_type::push},
            {"xpub", flame::socket_type::xpub},
            {"xsub", flame::socket_type::xsub},
            {"stream", flame::socket_type::stream}
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