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
  kPublish,
  kSubscribe,
  kPush,
  kPull,
  kDealer,
  kRouter,
  kServerPair,
  kClientPair
};

enum class Transport { kTcp, kInproc, kIpc, kPgm, kEpgm };

/* ZData: standard multipart message carrier within flame::pipe */
using ZData = zmq::multipart_t;

std::string transport2Str(Transport t);

class ZSocket : public std::enable_shared_from_this<ZSocket> {
public:
  ZSocket(const std::string &socket_id, Pattern pattern);
  virtual ~ZSocket();

  bool create(std::shared_ptr<ZPipe> pipeline = nullptr);
  bool join(Transport transport, const std::string &address = "localhost",
            int port = 5555);
  void close();

  // Callback: called with received ZData (topic frame already stripped for SUB)
  typedef std::function<void(ZData&)> CallbackFunc;
  bool setMessageCallback(CallbackFunc callback);

  // Send ZData (non-const: Send() consumes the frames)
  bool dispatch(ZData& data);

  std::string getId() const { return socket_id_; }

private:
  void startReceiverThread();
  void receiverWorker();

private:
  std::string socket_id_;
  Pattern pattern_;
  std::shared_ptr<zmq::socket_t> socket_;
  bool is_server_;
  bool is_created_;
  bool is_joined_;

  // Threading for receiver
  std::thread *worker_thread_;
  std::atomic<bool> stop_event_;
  CallbackFunc callback_;
};

class ZPipe : public std::enable_shared_from_this<ZPipe> {
public:
  static std::shared_ptr<ZPipe> instance();
  static void destroyInstance();

  // Initialize with io_threads
  bool init(int io_threads = 1);

  zmq::context_t *getContext();

  bool registerSocket(std::shared_ptr<ZSocket> socket);
  bool unregisterSocket(const std::string &socket_id);
  std::shared_ptr<ZSocket> getSocket(const std::string &socket_id);

  ~ZPipe();

private:
  ZPipe() = default;
  ZPipe(const ZPipe &) = delete;
  ZPipe &operator=(const ZPipe &) = delete;

private:
  static std::shared_ptr<ZPipe> instance_;
  static std::mutex mutex_;

  zmq::context_t *context_ = nullptr;
  std::map<std::string, std::shared_ptr<ZSocket>> sockets_;
  std::mutex socket_mutex_;
};

// Global helper functions
std::shared_ptr<ZPipe> createPipe(int io_threads = 1);
void destroyPipe();
std::shared_ptr<ZSocket> getSocket(const std::string &socket_id);

} // namespace pipe
} // namespace flame

#endif
