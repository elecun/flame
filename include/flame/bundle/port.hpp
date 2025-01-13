/**
 * @file port.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Bundle Data & Service Port
 * @version 0.1
 * @date 2024-06-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_BUNDLE_PORT_HPP_INCLUDED
#define FLAME_BUNDLE_PORT_HPP_INCLUDED

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <flame/arch/singleton.hpp>

using dataport = zmq::context_t;

namespace flame::bundle {

    class dataport : public flame::arch::singleton<dataport> {
        public:
            dataport(){
                _bundle_ctx = new zmq::context_t(1);
            }
            virtual ~dataport(){
                _bundle_ctx->close();
                delete _bundle_ctx;
            }

        protected:
            zmq::context_t* get_context() const {
                return _bundle_ctx;
            }

        private:
            zmq::context_t* _bundle_ctx { nullptr };
    }; /* class */

    class serviceport : public flame::arch::singleton<serviceport> {
        public:
            serviceport() {

            }
            virtual ~serviceport() {

            }

        protected:

        private:
        
    }; /* class */
}

#endif