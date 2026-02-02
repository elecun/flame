/**
 * @file provider.cc
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief State Provider Implementation
 * @version 0.1
 * @date 2024-07-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "provider.hpp"
#include <flame/def.hpp>
#include <flame/log.hpp>
#include <flame/config.hpp>
#include "manager.hpp"

#include <dep/json.hpp>
#include <chrono>

using json = nlohmann::json;

namespace flame {

    StateProvider::StateProvider():_run_service(false), _t_service(nullptr), _epgm_addr(def::FLAME_MONITOR_EPGM_ADDR_DEFAULT) {
        if(config.is_loaded()){
            if(config.get_config().contains(def::FLAME_CONF_MONITOR)){
                if(config.get_config()[def::FLAME_CONF_MONITOR].contains(def::FLAME_CONF_MONITOR_ADDR)){
                    _epgm_addr = config.get_config()[def::FLAME_CONF_MONITOR][def::FLAME_CONF_MONITOR_ADDR].get<std::string>();
                }
            }
        }
    }

    StateProvider::~StateProvider(){
        stop();
    }

    void StateProvider::start(){
        if(!_run_service){
            _run_service = true;
            _t_service = new std::thread(&StateProvider::_publish_loop, this);
        }
    }

    void StateProvider::stop(){
        if(_run_service){
            _run_service = false;
            if(_t_service && _t_service->joinable()){
                _t_service->join();
            }
            if(_t_service){
                delete _t_service;
                _t_service = nullptr;
            }
        }
    }

    void StateProvider::_publish_loop(){
        
        auto pipe = flame::pipe::create_pipe(1);
        auto sock = std::make_shared<flame::pipe::AsyncZSocket>("state_pub", flame::pipe::Pattern::PUBLISH);

        if(sock->create(pipe)){
            // Parse EPGM address
            std::string ipc_addr = _epgm_addr;
            std::string address = ipc_addr.substr(ipc_addr.find("://") + 3);
            
            // EPGM format: epgm://eth0;239.192.1.1:5555
            // zpipe join expects: transport, address (eth0;239.192.1.1), port (5555)
            // But checking zpipe.cc join implementation:
            // if transport != INPROC/IPC, conn_str = transport + "://" + address + ":" + port
            // This means we need to manually separate address and port from the definition if we use join() helper.
            // Let's parse it carefully.
            
            // _epgm_addr = "epgm://eth0;239.192.1.1:5555"
            // address part for zmq should be "eth0;239.192.1.1:5555" ? 
            // wait, zpipe::join takes address and port.
            // if we pass address="eth0;239.192.1.1", port=5555
            // zpipe makes "epgm://eth0;239.192.1.1:5555"
            
            std::string addr_part = address.substr(0, address.find_last_of(':'));
            int port = std::stoi(address.substr(address.find_last_of(':') + 1));

            if(sock->join(flame::pipe::Transport::EPGM, addr_part, port)){
                logger::info("State Provider Started : {}", _epgm_addr);

                while(_run_service){
                    // Gather info
                    json j_info;
                    j_info["status"] = "Active";
                    j_info["count"] = manager.get_component_count();
                    j_info["components"] = manager.get_component_info();
                    
                    std::string rep_str = j_info.dump();
                    
                    // Publish
                    sock->dispatch({rep_str});
                    
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            } else {
                logger::error("Failed to join EPGM multicast group");
            }
            sock->close();
        } else {
            logger::error("Failed to create publisher socket");
        }

        flame::pipe::destroy_pipe();
    }

    void StateProvider::start_subscribe(){
        if(!_run_service){
            _run_service = true;
            _t_service = new std::thread(&StateProvider::_subscribe_loop, this);
        }
    }

    void StateProvider::_subscribe_loop(){
        auto pipe = flame::pipe::create_pipe(1);
        auto sock = std::make_shared<flame::pipe::AsyncZSocket>("cli_subscriber", flame::pipe::Pattern::SUBSCRIBE);

        if(sock->create(pipe)){
            std::string ipc_addr = _epgm_addr;
            std::string address = ipc_addr.substr(ipc_addr.find("://") + 3);
            
            std::string addr_part = address.substr(0, address.find_last_of(':'));
            int port = std::stoi(address.substr(address.find_last_of(':') + 1));

            sock->set_message_callback([&](const std::vector<std::string>& msg){
                // placeholder for data reception
                if(!msg.empty()){
                    // console::info("Received : {}", msg[0]);
                }
            });

            if(sock->join(flame::pipe::Transport::EPGM, addr_part, port)){
                while(_run_service){
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            } else {
                logger::error("Failed to join EPGM multicast group");
            }
            sock->close();
        } else {
            logger::error("Failed to create subscriber socket");
        }
        flame::pipe::destroy_pipe();
    }

} /* namespace */
