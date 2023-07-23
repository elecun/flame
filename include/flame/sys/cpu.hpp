/**
 * @file cpu.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Target machine CPU info
 * @version 0.1
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _FLAME_SYS_CPU_HPP_
#define _FLAME_SYS_CPU_HPP_

#include <flame/arch/singleton.hpp>
#include <fstream>
#include <stdexcept>

using namespace std;

#if defined(__linux__)
#include <sys/sysinfo.h>

namespace flame::sys {

    class cpu {
        public:
            cpu() = default;
            virtual ~cpu() = default;
            

    }; /* class */

    namespace stat {
        class cpu : public flame::sys::cpu {
            public:
            cpu(const char* procfile = "/proc/stat"):_profile(procfile){

            }
            ~cpu() = default;

            int get_nprocs() { return ::get_nprocs(); } /* return number of processors */

            private:
            const char* _profile { nullptr };

            }; /* class */
        } /* perf namespace */
} /* namespace */

#endif // for linux

#endif