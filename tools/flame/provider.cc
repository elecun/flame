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
  auto sock = std::make_shared<flame::pipe::AsyncZSocket>(
      "state_pub", flame::pipe::Pattern::ROUTER);

  if(sock->create(pipe)){
    // Parse IPC address
    std::string ipc_addr = _ipc_addr;
    std::string address = ipc_addr.substr(ipc_addr.find("://") + 3);

    // server bind
    sock->set_message_callback([&](const std::vector<std::string> &msg) {
      if(!msg.empty()){
        // msg[0] : identity frame
        // msg[1] : empty frame
        // msg[2] : data

        std::string identity = msg[0];
        // logger::info("Received from client : {}", identity);

        // Gather info
        json j_info;
        j_info["status"] = "Active";
        j_info["count"] = manager.get_component_count();
        j_info["components"] = manager.get_component_info();
        std::string rep_str = j_info.dump();

        // Send Reply
        sock->dispatch({identity, "", rep_str});
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
  auto sock = std::make_shared<flame::pipe::AsyncZSocket>(
      "cli_dealer", flame::pipe::Pattern::DEALER);

  if(sock->create(pipe)){
    std::string ipc_addr = _ipc_addr;
    std::string address = ipc_addr.substr(ipc_addr.find("://") + 3);

    // client connect
    sock->set_message_callback([&](const std::vector<std::string> &msg) {
      if(!msg.empty()){
        // msg[0] : empty frame
        // msg[1] : data

        if(msg.size() > 1 && msg[1].size() > 0){
          // logger::info("Received : {}", msg[1]);
          std::cout << msg[1] << std::endl;
        }
      }
    });

    if(sock->join(flame::pipe::Transport::IPC, address)){
      sock->dispatch({"", "Hello"}); // Request
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
