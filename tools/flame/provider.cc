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
    : run_service_(false), service_thread_(nullptr),
      ipc_addr_(def::kFlameMonitorIpcAddrDefault) {
  if(CONFIG.isLoaded()){
    if(CONFIG.getConfig().contains(def::kFlameConfMonitor)){
      if(CONFIG.getConfig()[def::kFlameConfMonitor].contains(
              def::kFlameConfMonitorAddr)){
        ipc_addr_ = CONFIG
                        .getConfig()[def::kFlameConfMonitor]
                                     [def::kFlameConfMonitorAddr]
                        .get<std::string>();
      }
    }
  }
}

StateProvider::~StateProvider() { stop(); }

void StateProvider::start(){
  if(!run_service_){
    run_service_ = true;
    service_thread_ = new std::thread(&StateProvider::publishLoop, this);
  }
}

void StateProvider::stop(){
  if(run_service_){
    run_service_ = false;
    if(service_thread_ && service_thread_->joinable()){
      service_thread_->join();
    }
    if(service_thread_){
      delete service_thread_;
      service_thread_ = nullptr;
    }
  }
}

void StateProvider::publishLoop(){

  auto pipe = flame::pipe::ZPipe::instance();
  auto sock = std::make_shared<flame::pipe::ZSocket>(
      "state_pub", flame::pipe::Pattern::kRouter);

  if(sock->create(pipe)){
    // Parse IPC address
    std::string ipc_addr = ipc_addr_;
    std::string address = ipc_addr.substr(ipc_addr.find("://") + 3);

    // server bind
    sock->setMessageCallback([&](zmq::multipart_t& msg) {
      if(!msg.empty()){
        // frame[0] : identity frame
        // frame[1] : empty frame
        // frame[2] : data
        std::string identity = msg.popstr();
        msg.popstr(); // empty delimiter

        // Gather info
        json j_info;
        j_info["status"] = "Active";
        j_info["count"] = MANAGER.getComponentCount();
        j_info["components"] = MANAGER.getComponentInfo();
        std::string rep_str = j_info.dump();

        // Send Reply
        zmq::multipart_t reply;
        reply.addstr(identity);
        reply.addstr("");
        reply.addstr(rep_str);
        sock->dispatch(reply);
      }
    });

    if(sock->join(flame::pipe::Transport::kIpc, address)){
      logger::info("State Provider Started : {}", ipc_addr_);

      while(run_service_){
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
  auto pipe = flame::pipe::ZPipe::instance();
  auto sock = std::make_shared<flame::pipe::ZSocket>(
      "cli_dealer", flame::pipe::Pattern::kDealer);

  if(sock->create(pipe)){
    std::string ipc_addr = ipc_addr_;
    std::string address = ipc_addr.substr(ipc_addr.find("://") + 3);

    // client connect
    sock->setMessageCallback([&](zmq::multipart_t& msg) {
      if(!msg.empty()){
        // frame[0] : empty frame
        // frame[1] : data
        msg.popstr(); // empty delimiter
        if(!msg.empty()){
          std::cout << msg.popstr() << std::endl;
        }
      }
    });

    if(sock->join(flame::pipe::Transport::kIpc, address)){
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
