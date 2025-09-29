/**
 * @file singleton.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Singleton Template Class
 * @version 0.2
 * @date 2024-05-24
 * 
 * @copyright Copyright (c) 2023, 2024
 * 
 */


#ifndef _FLAME_ARCH_SINGLETON_HPP_
#define _FLAME_ARCH_SINGLETON_HPP_

namespace flame::arch {

    template<class T>
    class singleton {
        public:
            static T& instance() {
                static T inst;
                return inst;
            }

        protected:
            singleton() = default;
            ~singleton() = default;
            singleton(const singleton&) = delete;
            singleton& operator=(const singleton&) = delete;
            singleton(singleton&&) = delete;
            singleton& operator=(singleton&&) = delete;
    };

} // namespace flame::arch

#endif