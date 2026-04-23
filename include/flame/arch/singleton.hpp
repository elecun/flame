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


#ifndef FLAME_ARCH_SINGLETON_HPP_INCLUDED
#define FLAME_ARCH_SINGLETON_HPP_INCLUDED

namespace flame::arch {

    template<class T>
    class Singleton {
        public:
            static T& instance() {
                static T inst;
                return inst;
            }

        protected:
            Singleton() = default;
            ~Singleton() = default;
            Singleton(const Singleton&) = delete;
            Singleton& operator=(const Singleton&) = delete;
            Singleton(Singleton&&) = delete;
            Singleton& operator=(Singleton&&) = delete;
    };

} // namespace flame::arch

#endif