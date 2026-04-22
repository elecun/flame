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
enum class dtype_status : int { STOPPED = 0, WORKING };

class driver;
class object : public interface {
  friend class flame::component::driver;

public:
  object() = default;
  virtual ~object() = default;

  const char *get_name() const { return _name.c_str(); }
  const dtype_status get_status() const { return _status; }

protected:
  /* get component profile */
  component::profile *get_profile() const { return _profile.get(); }

  /* get zsocket from portname */
  std::shared_ptr<flame::pipe::zsocket> get_port(const string portname) {
    if (_socket_map.contains(portname))
      return _socket_map[portname];
    else
      throw std::runtime_error(fmt::format("{} does not exist.", portname));
    return nullptr;
  }

  /* dispatch data directly by portname - zdata is consumed by send */
  bool dispatch(const string portname, flame::component::zdata& data) {
    auto sock = get_port(portname);
    if (sock) {
      return sock->dispatch(data);
    }
    return false;
  }

private:
  void set_status(dtype_status s) { _status = s; }

  /* create zsocket port */
  std::shared_ptr<flame::pipe::zsocket> create_zsocket(
      const string socket_name,
      const flame::socket_type sock_type,
      const string transport_str,
      const string address,
      int port) {
      
      flame::pipe::Pattern pattern = flame::pipe::Pattern::PUBLISH;
      switch(sock_type) {
          case flame::socket_type::pub: pattern = flame::pipe::Pattern::PUBLISH; break;
          case flame::socket_type::sub: pattern = flame::pipe::Pattern::SUBSCRIBE; break;
          case flame::socket_type::push: pattern = flame::pipe::Pattern::PUSH; break;
          case flame::socket_type::pull: pattern = flame::pipe::Pattern::PULL; break;
          case flame::socket_type::req: pattern = flame::pipe::Pattern::CLIENT_PAIR; break;
          case flame::socket_type::rep: pattern = flame::pipe::Pattern::SERVER_PAIR; break;
          default: break;
      }

      flame::pipe::Transport transport = flame::pipe::Transport::TCP;
      if (transport_str == "epgm") transport = flame::pipe::Transport::EPGM;
      else if (transport_str == "pgm") transport = flame::pipe::Transport::PGM;
      else if (transport_str == "ipc") transport = flame::pipe::Transport::IPC;
      else if (transport_str == "inproc") transport = flame::pipe::Transport::INPROC;

      auto socket = std::make_shared<flame::pipe::zsocket>(socket_name, pattern);
      if (socket->create()) {
          if (socket->join(transport, address, port)) {
              _socket_map[socket_name] = socket;
              logger::info("[{}] Created zsocket port({}) : {}://{}:{}", _name,
                           static_cast<int>(sock_type), transport_str, address, port);
              return socket;
          }
      }
      return nullptr;
  }

  /* clear all pipe data to close immediately */
  void close_port() {
    for (auto &[key, sock] : _socket_map) {
      if (sock) {
        sock->close();
      }
    }
  }

private:
  dtype_status _status{dtype_status::STOPPED};
  string _name = {"noname"};
  unique_ptr<profile> _profile;

  std::unordered_map<string, std::shared_ptr<flame::pipe::zsocket>> _socket_map;

}; /* class */

typedef flame::component::object *(*create_component)(void);
typedef void (*release_component)(void);

#define EXPORT_COMPONENT_API                                                   \
  extern "C" {                                                                 \
  flame::component::object *create(void);                                      \
  void release(void);                                                          \
  }

} // namespace flame::component

#endif