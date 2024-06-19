/**
 * @file bundle.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Bundle (Component package)
 * @version 0.1
 * @date 2024-06-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_BUNDLE_BUNDLE_HPP_INCLUDED
#define FLAME_BUNDLE_BUNDLE_HPP_INCLUDED

#include <map>
#include <string>
#include <flame/util/uuid.hpp>

using namespace std;

namespace flame::bundle {

    class object {
        public:
            object() = default;
            ~object() = default;

    }; /* class */

    class interface {
        public:
            interface() = default;
            virtual ~interface() = default;

    }; /* class */

    

} /* namespace */

#endif