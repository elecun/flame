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

namespace flame::bundle {

    class DataPort : public flame::arch::Singleton<DataPort> {
        public:
            DataPort(){
                bundle_ctx_ = new zmq::context_t(1);
            }
            virtual ~DataPort(){
                bundle_ctx_->close();
                delete bundle_ctx_;
            }

        protected:
            zmq::context_t* getContext() const {
                return bundle_ctx_;
            }

        private:
            zmq::context_t* bundle_ctx_ { nullptr };
    }; /* class */

    class ServicePort : public flame::arch::Singleton<ServicePort> {
        public:
            ServicePort() {

            }
            virtual ~ServicePort() {

            }

        protected:

        private:
        
    }; /* class */
}

#endif