/**
 * @file driver.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component Driver
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 */


#ifndef FLAME_COMPONENT_DRIVER_HPP_INCLUDED
#define FLAME_COMPONENT_DRIVER_HPP_INCLUDED

#include <flame/component/object.hpp>
#include <flame/component/interface.hpp>
#include <flame/component/port.hpp>
#include <filesystem>
#include <memory>
#include <csignal>
#include <chrono>
#include <type_traits>
#include <thread>
#include <atomic>
#include <zmq.hpp> //libzmq3-dev
#include <zmq_addon.hpp>

using namespace std;
namespace fs = std::filesystem;

namespace flame::component {
    class Driver : public component::Interface {
        public:
            Driver(fs::path component_path); //without extension
            virtual ~Driver();

            /* common component interfaces */
            bool onInit() override;
            void onLoop() override;
            void onClose() override;
            void onData(flame::component::ZData& data) override;

            /* get component name */
            const char* getName() {
                if(component_impl_)
                    return component_impl_->getName();
                return nullptr;
            }

            /* get component type */
            string getType() {
                if(component_impl_) {
                   auto profile = component_impl_->getProfile();
                   if(profile) {
                        string type_s = profile->getDumped("type");
                        if(type_s != "{}") {
                            if(profile->raw().contains("type")) {
                                return profile->raw()["type"].get<string>();
                            }
                        }
                   }
                }
                return "unknown";
            }

            /* get component status */
            string getStatusStr() {
                if(component_impl_) {
                    if(component_impl_->getStatus() == DTypeStatus::kStopped) return "Stopped";
                    if(component_impl_->getStatus() == DTypeStatus::kWorking) return "Working";
                }
                return "Unknown";
            }

        private:
            /**
             * @brief component file(*.comp) load
             * 
             * @param component_path 
             * @return true if success
             * @return false if failed
             */
            bool load(fs::path component_path);

            /**
             * @brief unload the component file(*.comp)
             * 
             */
            void unload();

            /**
             * @brief concrete periodic process
             * 
             */
            void doCycle();
            
            /**
             * @brief Set the rt timer object
             * 
             * @param nsec time period
             */
            void setRTTimer(unsigned long long nsec);

        private:
            flame::component::Object* component_impl_ { nullptr };
            void* component_handle_ { nullptr };
            struct sigevent signal_event_;
            struct itimerspec time_spec_;
            std::thread* ptr_thread_ = nullptr;
            timer_t timer_id_ {0};
            int signal_id_ { 0 };
            std::atomic<bool> is_running_ { false };



    }; /* class */
    
} /* namespace */

#endif