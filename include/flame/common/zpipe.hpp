/**
 * @file zpipe.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief ZMQ Pipeline Module (ZPipe)
 */

#ifndef FLAME_PIPE_ZPIPE_HPP_INCLUDED
#define FLAME_PIPE_ZPIPE_HPP_INCLUDED

#include <atomic>
#include <flame/log.hpp>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace flame {
namespace pipe {

class ZPipe; // Forward declaration

enum class Pattern {
  PUBLISH,
  SUBSCRIBE,
  PUSH,
  PULL,
  DEALER,
  ROUTER,
  SERVER_PAIR,
  CLIENT_PAIR
};

enum class Transport { TCP, INPROC, IPC, PGM, EPGM };

/* zdata: standard multipart message carrier within flame::pipe */
using zdata = zmq::multipart_t;

std::string transport2str(Transport t);

class zsocket : public std::enable_shared_from_this<zsocket> {
public:
  zsocket(const std::string &socket_id, Pattern pattern);
  virtual ~zsocket();

  bool create(std::shared_ptr<ZPipe> pipeline = nullptr);
  bool join(Transport transport, const std::string &address = "localhost",
            int port = 5555);
  void close();

  // Callback: called with received zdata (topic frame already stripped for SUB)
  typedef std::function<void(zdata&)> callback_t;
  bool set_message_callback(callback_t callback);

  // Send zdata (non-const: send() consumes the frames)
  bool dispatch(zdata& data);

  std::string get_id() const { return _socket_id; }

private:
  void _start_receiver_thread();
  void _receiver_worker();

private:
  std::string _socket_id;
  Pattern _pattern;
  std::shared_ptr<zmq::socket_t> _socket;
  bool _is_server;
  bool _is_created;
  bool _is_joined;

  // Threading for receiver
  std::thread *_worker_thread;
  std::atomic<bool> _stop_event;
  callback_t _callback;
};

class ZPipe : public std::enable_shared_from_this<ZPipe> {
public:
  static std::shared_ptr<ZPipe> get_instance();
  static void destroy_instance();

  // Initialize with io_threads
  bool init(int io_threads = 1);

  zmq::context_t *get_context();

  bool register_socket(std::shared_ptr<zsocket> socket);
  bool unregister_socket(const std::string &socket_id);
  std::shared_ptr<zsocket> get_socket(const std::string &socket_id);

  ~ZPipe();

private:
  ZPipe() = default;
  ZPipe(const ZPipe &) = delete;
  ZPipe &operator=(const ZPipe &) = delete;

private:
  static std::shared_ptr<ZPipe> _instance;
  static std::mutex _mutex;

  zmq::context_t *_context = nullptr;
  std::map<std::string, std::shared_ptr<zsocket>> _sockets;
  std::mutex _socket_mutex;
};

// Global helper functions
std::shared_ptr<ZPipe> create_pipe(int io_threads = 1);
void destroy_pipe();
std::shared_ptr<zsocket> get_socket(const std::string &socket_id);

} // namespace pipe
} // namespace flame

#endif
