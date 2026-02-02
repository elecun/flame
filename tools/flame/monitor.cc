/**
 * @file monitor.cc
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Flame Monitor Service Implementation
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "monitor.hpp"
#include <flame/def.hpp>
#include <flame/log.hpp>
#include "manager.hpp"

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <dep/json.hpp>

using json = nlohmann::json;

namespace flame {

    monitor_service::monitor_service():_run_service(false), _ipc_addr(def::FLAME_MONITOR_IPC_ADDR) {

    }

    monitor_service::~monitor_service(){
        stop_service();
    }

    void monitor_service::start_service(){
        if(!_run_service){
            _run_service = true;
            _t_service = new std::thread(&monitor_service::_service_loop, this);
        }
    }

    void monitor_service::stop_service(){
        if(_run_service){
            _run_service = false;
            if(_t_service->joinable()){
                _t_service->join();
            }
            delete _t_service;
            _t_service = nullptr;
        }
    }

    void monitor_service::_service_loop(){
        
        zmq::context_t ctx(1);
        zmq::socket_t sock(ctx, ZMQ_REP);
        sock.set(zmq::sockopt::linger, 0); // linger 0

        try{
            sock.bind(_ipc_addr);
            logger::info("Monitor Service Started : {}", _ipc_addr);

            zmq::pollitem_t items[] = { { sock, 0, ZMQ_POLLIN, 0 } };
            
            while(_run_service){
                
                zmq::poll(&items[0], 1, 100); // 100ms timeout
                
                if (items[0].revents & ZMQ_POLLIN) {
                    zmq::message_t request;
                    if(sock.recv(request, zmq::recv_flags::none)){
                        std::string req_str = request.to_string();
                        
                        if(req_str == "INFO"){
                            // Get info from manager
                            json j_info;
                            j_info["count"] = manager.get_component_count();
                            j_info["components"] = manager.get_component_list();
                            
                            std::string rep_str = j_info.dump();
                            sock.send(zmq::buffer(rep_str), zmq::send_flags::none);
                        }
                        else if(req_str == "STATUS"){
                            // Get status from manager
                            json j_info;
                            j_info["status"] = "Active";
                            j_info["count"] = manager.get_component_count();
                            j_info["components"] = manager.get_component_info();
                            
                            std::string rep_str = j_info.dump();
                            sock.send(zmq::buffer(rep_str), zmq::send_flags::none);
                        }
                        else {
                            sock.send(zmq::buffer("Unknown Command"), zmq::send_flags::none);
                        }
                    }
                }
            }

            sock.close();
            ctx.close();
        }
        catch(const zmq::error_t& e){
            logger::warn("Monitor Service Error : {}", e.what());
        }
    }

} /* namespace */
