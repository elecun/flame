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

#include <string>
#include <stdexcept>
#include <unordered_map>

using namespace std;

namespace flame {
    


    enum class SocketType : int {
        Pair = 0,
        Pub,
        Sub,
        Req,
        Rep,
        Dealer,
        Router,
        Pull,
        Push,
        XPub,
        XSub,
        Stream
    };


    inline SocketType str2Type(const std::string& str_type) {
        const std::unordered_map<std::string, SocketType> s_type = {
            {"pair", flame::SocketType::Pair},
            {"pub", flame::SocketType::Pub},
            {"sub", flame::SocketType::Sub},
            {"req", flame::SocketType::Req},
            {"rep", flame::SocketType::Rep},
            {"dealer", flame::SocketType::Dealer},
            {"router", flame::SocketType::Router},
            {"pull", flame::SocketType::Pull},
            {"push", flame::SocketType::Push},
            {"xpub", flame::SocketType::XPub},
            {"xsub", flame::SocketType::XSub},
            {"stream", flame::SocketType::Stream}
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