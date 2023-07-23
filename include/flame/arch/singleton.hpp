/**
 * @file singleton.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Singleton Template Class
 * @version 0.1
 * @date 2023-07-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _FLAME_ARCH_SINGLETON_HPP_
#define _FLAME_ARCH_SINGLETON_HPP_

#include <utility>

namespace flame::arch {

        template<class _t>
        class singleton {
            public:
            static _t* instance(){
                if(!_instance){
                    _instance = new _t();
                }
                return _instance;
            }

            static void terminate() {
                if(_instance){
                    delete _instance;
                    _instance = nullptr;
                }
            }

            protected:
            singleton() = default;
            ~singleton() = default;
            singleton(const singleton&) = delete;   //for noncopyable
            singleton& operator=(const singleton&) = delete; //for noncopyable

            private:
            static _t* _instance;
        };

    template <class _t> _t* singleton<_t>::_instance = nullptr; //initialize

} //namespace oe

#endif