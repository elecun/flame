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
#ifndef FLAME_SYS_CPU_HPP_INCLUDED
#define FLAME_SYS_CPU_HPP_INCLUDED

#include <flame/arch/singleton.hpp>
#include <fstream>
#include <stdexcept>

#if defined(__linux__)
#include <sys/sysinfo.h>

namespace flame::sys {

    class Cpu {
        public:
            Cpu() = default;
            virtual ~Cpu() = default;
            

    }; /* class */

    namespace stat {
        class Cpu : public flame::sys::Cpu {
            public:
            Cpu(const char* procfile = "/proc/stat"):profile_(procfile){

            }
            ~Cpu() = default;

            int getNProcs() { return ::get_nprocs(); } /* return number of processors */

            private:
            const char* profile_ { nullptr };

            }; /* class */
        } /* stat namespace */
} /* namespace */

#endif // for linux

#endif