/**
 * @file object.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component Object (*.comp)
 * @version 0.1
 * @date 2024-06-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef FLAME_COMPONENT_OBJECT_HPP_INCLUDED
#define FLAME_COMPONENT_OBJECT_HPP_INCLUDED

#include <atomic>
#include <chrono>
#include <filesystem>
#include <flame/component/interface.hpp>
#include <flame/component/port.hpp>
#include <flame/component/profile.hpp>
#include <flame/component/type.hpp>
#include <flame/def.hpp>
#include <flame/log.hpp>
#include <memory>
#include <string>
#include <thread>
#include <flame/common/zpipe.hpp>

using namespace std;
using path = std::filesystem::path;

namespace flame::component {
enum class DTypeStatus : int { kStopped = 0, kWorking };

class Driver;
class Object : public Interface {
  friend class flame::component::Driver;

public:
  Object() = default;
  virtual ~Object() = default;

  const char *getName() const { return name_.c_str(); }
  const DTypeStatus getStatus() const { return status_; }

protected:
  /* get component profile */
  component::Profile *getProfile() const { return profile_.get(); }

  /* get ZSocket from portname */
  std::shared_ptr<flame::pipe::ZSocket> getPort(const string portname) {
    if (socket_map_.contains(portname))
      return socket_map_[portname];
    else
      throw std::runtime_error(fmt::format("{} does not exist.", portname));
    return nullptr;
  }

  /* dispatch data directly by portname - ZData is consumed by send */
  bool dispatch(const string portname, flame::component::ZData& data) {
    auto sock = getPort(portname);
    if (sock) {
      return sock->dispatch(data);
    }
    return false;
  }

private:
  void setStatus(DTypeStatus s) { status_ = s; }

  /* create ZSocket port */
  std::shared_ptr<flame::pipe::ZSocket> createZSocket(
      const string socket_name,
      const flame::SocketType sock_type,
      const string transport_str,
      const string address,
      int port) {
      
      flame::pipe::Pattern pattern = flame::pipe::Pattern::kPublish;
      switch(sock_type) {
          case flame::SocketType::kPub: pattern = flame::pipe::Pattern::kPublish; break;
          case flame::SocketType::kSub: pattern = flame::pipe::Pattern::kSubscribe; break;
          case flame::SocketType::kPush: pattern = flame::pipe::Pattern::kPush; break;
          case flame::SocketType::kPull: pattern = flame::pipe::Pattern::kPull; break;
          case flame::SocketType::kReq: pattern = flame::pipe::Pattern::kClientPair; break;
          case flame::SocketType::kRep: pattern = flame::pipe::Pattern::kServerPair; break;
          default: break;
      }

      flame::pipe::Transport transport = flame::pipe::Transport::kTcp;
      if (transport_str == "epgm") transport = flame::pipe::Transport::kEpgm;
      else if (transport_str == "pgm") transport = flame::pipe::Transport::kPgm;
      else if (transport_str == "ipc") transport = flame::pipe::Transport::kIpc;
      else if (transport_str == "inproc") transport = flame::pipe::Transport::kInproc;

      auto socket = std::make_shared<flame::pipe::ZSocket>(socket_name, pattern);
      if (socket->create()) {
          if (socket->join(transport, address, port)) {
              socket_map_[socket_name] = socket;
              logger::info("[{}] Created ZSocket port({}) : {}://{}:{}", name_,
                           static_cast<int>(sock_type), transport_str, address, port);
              return socket;
          }
      }
      return nullptr;
  }

  /* clear all pipe data to close immediately */
  void closePort() {
    for (auto &[key, sock] : socket_map_) {
      if (sock) {
        sock->close();
      }
    }
  }

private:
  DTypeStatus status_{DTypeStatus::kStopped};
  string name_ = {"noname"};
  unique_ptr<Profile> profile_;

  std::unordered_map<string, std::shared_ptr<flame::pipe::ZSocket>> socket_map_;

}; /* class */

typedef flame::component::Object *(*CreateComponentFunc)(void);
typedef void (*ReleaseComponentFunc)(void);

#define EXPORT_COMPONENT_API                                                   \
  extern "C" {                                                                 \
  flame::component::Object *Create(void);                                      \
  void Release(void);                                                          \
  }

} // namespace flame::component

#endif