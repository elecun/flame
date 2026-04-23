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
        kPair = 0,
        kPub,
        kSub,
        kReq,
        kRep,
        kDealer,
        kRouter,
        kPull,
        kPush,
        kXPub,
        kXSub,
        kStream
    };


    inline SocketType str2Type(const std::string& str_type) {
        const std::unordered_map<std::string, SocketType> s_type = {
            {"pair", flame::SocketType::kPair},
            {"pub", flame::SocketType::kPub},
            {"sub", flame::SocketType::kSub},
            {"req", flame::SocketType::kReq},
            {"rep", flame::SocketType::kRep},
            {"dealer", flame::SocketType::kDealer},
            {"router", flame::SocketType::kRouter},
            {"pull", flame::SocketType::kPull},
            {"push", flame::SocketType::kPush},
            {"xpub", flame::SocketType::kXPub},
            {"xsub", flame::SocketType::kXSub},
            {"stream", flame::SocketType::kStream}
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