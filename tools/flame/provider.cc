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
#include "manager.hpp"
#include <flame/config.hpp>
#include <flame/def.hpp>
#include <flame/log.hpp>

#include <chrono>
#include <dep/json.hpp>

using json = nlohmann::json;

namespace flame {

StateProvider::StateProvider()
    : _run_service(false), _t_service(nullptr),
      _ipc_addr(def::FLAME_MONITOR_IPC_ADDR_DEFAULT) {
  if(config.is_loaded()){
    if(config.get_config().contains(def::FLAME_CONF_MONITOR)){
      if(config.get_config()[def::FLAME_CONF_MONITOR].contains(
              def::FLAME_CONF_MONITOR_ADDR)){
        _ipc_addr = config
                        .get_config()[def::FLAME_CONF_MONITOR]
                                     [def::FLAME_CONF_MONITOR_ADDR]
                        .get<std::string>();
      }
    }
  }
}

StateProvider::~StateProvider() { stop(); }

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

  auto pipe = flame::pipe::ZPipe::get_instance();
  auto sock = std::make_shared<flame::pipe::zsocket>(
      "state_pub", flame::pipe::Pattern::ROUTER);

  if(sock->create(pipe)){
    // Parse IPC address
    std::string ipc_addr = _ipc_addr;
    std::string address = ipc_addr.substr(ipc_addr.find("://") + 3);

    // server bind
    sock->set_message_callback([&](zmq::multipart_t& msg) {
      if(!msg.empty()){
        // frame[0] : identity frame
        // frame[1] : empty frame
        // frame[2] : data
        std::string identity = msg.popstr();
        msg.popstr(); // empty delimiter

        // Gather info
        json j_info;
        j_info["status"] = "Active";
        j_info["count"] = manager.get_component_count();
        j_info["components"] = manager.get_component_info();
        std::string rep_str = j_info.dump();

        // Send Reply
        zmq::multipart_t reply;
        reply.addstr(identity);
        reply.addstr("");
        reply.addstr(rep_str);
        sock->dispatch(reply);
      }
    });

    if(sock->join(flame::pipe::Transport::IPC, address)){
      logger::info("State Provider Started : {}", _ipc_addr);

      while(_run_service){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    } else {
      logger::error("Failed to bind IPC address");
    }
    sock->close();
  } else {
    logger::error("Failed to create router socket");
  }
}

void StateProvider::connect(){
  auto pipe = flame::pipe::ZPipe::get_instance();
  auto sock = std::make_shared<flame::pipe::zsocket>(
      "cli_dealer", flame::pipe::Pattern::DEALER);

  if(sock->create(pipe)){
    std::string ipc_addr = _ipc_addr;
    std::string address = ipc_addr.substr(ipc_addr.find("://") + 3);

    // client connect
    sock->set_message_callback([&](zmq::multipart_t& msg) {
      if(!msg.empty()){
        // frame[0] : empty frame
        // frame[1] : data
        msg.popstr(); // empty delimiter
        if(!msg.empty()){
          std::cout << msg.popstr() << std::endl;
        }
      }
    });

    if(sock->join(flame::pipe::Transport::IPC, address)){
      zmq::multipart_t req;
      req.addstr("");      // empty delimiter
      req.addstr("Hello"); // Request
      sock->dispatch(req);
      std::this_thread::sleep_for(
          std::chrono::milliseconds(1000)); // wait for reply
    } else {
      logger::error("Failed to connect IPC address");
    }
    sock->close();
  } else {
    logger::error("Failed to create dealer socket");
  }
}

} // namespace flame
