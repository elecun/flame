/**
 * @file zpipe.cc
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief ZMQ Pipeline Module (ZPipe) Implementation
 */

#include "zpipe.hpp"
#include <algorithm>
#include <iostream>
#include <zmq_addon.hpp>

namespace flame {
namespace pipe {

// -------------------------------------------------------------------------
// Helper
// -------------------------------------------------------------------------
std::string pattern2Str(Pattern p) {
  switch (p) {
  case Pattern::kPublish:
    return "publish";
  case Pattern::kSubscribe:
    return "subscribe";
  case Pattern::kPush:
    return "push";
  case Pattern::kPull:
    return "pull";
  case Pattern::kDealer:
    return "dealer";
  case Pattern::kRouter:
    return "router";
  case Pattern::kServerPair:
    return "server_pair";
  case Pattern::kClientPair:
    return "client_pair";
  default:
    return "unknown";
  }
}

std::string transport2Str(Transport t) {
  switch (t) {
  case Transport::kTcp:
    return "tcp";
  case Transport::kInproc:
    return "inproc";
  case Transport::kIpc:
    return "ipc";
  case Transport::kPgm:
    return "pgm";
  case Transport::kEpgm:
    return "epgm";
  default:
    return "unknown";
  }
}

// -------------------------------------------------------------------------
// ZSocket Implementation
// -------------------------------------------------------------------------

ZSocket::ZSocket(const std::string &socket_id, Pattern pattern)
    : socket_id_(socket_id), pattern_(pattern), socket_(nullptr),
      is_server_(false), is_created_(false), is_joined_(false),
      worker_thread_(nullptr), stop_event_(false), callback_(nullptr) {}

ZSocket::~ZSocket() { close(); }

bool ZSocket::create(std::shared_ptr<ZPipe> pipeline) {
  if (is_created_) {
    logger::warn("Socket {} already created", socket_id_);
    return true;
  }

  try {
    auto pipe = pipeline ? pipeline : ZPipe::instance();
    auto context = pipe->getContext();
    if (!context) {
      logger::error("ZPipe context is null");
      return false;
    }

    switch (pattern_) {
    case Pattern::kPublish:
      socket_ =
          std::make_shared<zmq::socket_t>(*context, zmq::socket_type::pub);
      break;
    case Pattern::kSubscribe:
      socket_ =
          std::make_shared<zmq::socket_t>(*context, zmq::socket_type::sub);
      break;
    case Pattern::kPush:
      socket_ =
          std::make_shared<zmq::socket_t>(*context, zmq::socket_type::push);
      break;
    case Pattern::kPull:
      socket_ =
          std::make_shared<zmq::socket_t>(*context, zmq::socket_type::pull);
      break;
    case Pattern::kRouter:
      socket_ =
          std::make_shared<zmq::socket_t>(*context, zmq::socket_type::router);
      break;
    case Pattern::kDealer:
      socket_ =
          std::make_shared<zmq::socket_t>(*context, zmq::socket_type::dealer);
      break;
    case Pattern::kServerPair:
    case Pattern::kClientPair:
      socket_ =
          std::make_shared<zmq::socket_t>(*context, zmq::socket_type::pair);
      break;
    }

    // Configure socket options
    socket_->set(zmq::sockopt::linger, 0);

    if (pattern_ == Pattern::kSubscribe || pattern_ == Pattern::kPull ||
        pattern_ == Pattern::kDealer || pattern_ == Pattern::kServerPair) {
      socket_->set(zmq::sockopt::rcvhwm, 100);
      socket_->set(zmq::sockopt::rcvtimeo, 100);
      socket_->set(zmq::sockopt::reconnect_ivl, 500);
    }

    if (pattern_ == Pattern::kPublish || pattern_ == Pattern::kPush ||
        pattern_ == Pattern::kRouter || pattern_ == Pattern::kClientPair) {
      socket_->set(zmq::sockopt::sndhwm, 100);
      socket_->set(zmq::sockopt::sndtimeo, 100);
      socket_->set(zmq::sockopt::reconnect_ivl, 500);
    }

    pipe->registerSocket(shared_from_this());

    is_created_ = true;
    logger::debug("Created socket {} with pattern {}", socket_id_,
                  pattern2Str(pattern_));

    // Auto-subscribe if pattern is kSubscribe
    if (pattern_ == Pattern::kSubscribe) {
      socket_->set(zmq::sockopt::subscribe, socket_id_);
      logger::debug("Socket {} auto-subscribed to topic '{}'", socket_id_,
                    socket_id_);
    }

    return true;
  } catch (const zmq::error_t &e) {
    logger::error("Failed to create socket {}: {}", socket_id_, e.what());
    return false;
  }
}

bool ZSocket::join(Transport transport, const std::string &address,
                        int port) {
  if (!is_created_ || !socket_) {
    logger::error("Socket {} not created", socket_id_);
    return false;
  }

  if (is_joined_) {
    logger::warn("Socket {} already joined", socket_id_);
    return true;
  }

  try {
    std::string transport_str = transport2Str(transport);
    std::string conn_str;
    if (transport == Transport::kInproc || transport == Transport::kIpc) {
      conn_str = transport_str + "://" + address;
    } else {
      conn_str = transport_str + "://" + address + ":" + std::to_string(port);
    }

    if (pattern_ == Pattern::kPublish || pattern_ == Pattern::kPull ||
        pattern_ == Pattern::kRouter || pattern_ == Pattern::kServerPair) {
      is_server_ = true;
      socket_->bind(conn_str);
      logger::debug("Socket {} ({}) bound to: {}", socket_id_,
                    pattern2Str(pattern_), conn_str);
    } else {
      is_server_ = false;
      socket_->connect(conn_str);
      logger::debug("Socket {} ({}) connected to: {}", socket_id_,
                    pattern2Str(pattern_), conn_str);
    }

    is_joined_ = true;

    if ((pattern_ == Pattern::kSubscribe || pattern_ == Pattern::kPull ||
         pattern_ == Pattern::kRouter || pattern_ == Pattern::kDealer ||
         pattern_ == Pattern::kClientPair ||
         pattern_ == Pattern::kServerPair) &&
        callback_) {
      startReceiverThread();
    }

    return true;

  } catch (const zmq::error_t &e) {
    logger::error("Failed to join socket {}: {}", socket_id_, e.what());
    return false;
  }
}

void ZSocket::close() {
  // Idempotent: skip if already closed
  if (!is_created_ && !socket_ && !worker_thread_) {
    return;
  }

  if (!stop_event_) {
    stop_event_ = true;
  }

  // Close the socket first to interrupt zmq::poll waiting in the worker thread
  if (socket_) {
    socket_->close();
    // Do NOT set socket_ to nullptr yet, as worker_thread_ might still be
    // accessing it
  }

  if (worker_thread_ && worker_thread_->joinable()) {
    worker_thread_->join();
    delete worker_thread_;
    worker_thread_ = nullptr;
  }

  socket_ = nullptr;

  is_created_ = false;
  is_joined_ = false;
  logger::debug("Destroyed socket {}", socket_id_);
}

bool ZSocket::setMessageCallback(CallbackFunc callback) {
  callback_ = callback;
  return true;
}

bool ZSocket::dispatch(ZData& data) {
  if (!socket_ || !is_joined_) {
    logger::error("Socket not joined");
    return false;
  }

  try {
    // Auto-prepend topic if pattern is kPublish
    if (pattern_ == Pattern::kPublish) {
      data.pushmem(socket_id_.data(), socket_id_.size());
    }
    data.send(*socket_);
    return true;
  } catch (const zmq::error_t &e) {
    if (e.num() == EAGAIN) {
      logger::warn("Send would block");
    } else {
      logger::error("Failed to dispatch data: {}", e.what());
    }
    return false;
  }
}

void ZSocket::startReceiverThread() {
  if (worker_thread_)
    return;

  stop_event_ = false;
  worker_thread_ = new std::thread(&ZSocket::receiverWorker, this);
  logger::debug("Receiver thread started for socket {}", socket_id_);
}

void ZSocket::receiverWorker() {
  while (!stop_event_) {
    try {
      zmq::pollitem_t items[] = {
          {static_cast<void *>(*socket_), 0, ZMQ_POLLIN, 0}};
      zmq::poll(&items[0], 1, std::chrono::milliseconds(1000)); // 1 sec timeout

      if (items[0].revents & ZMQ_POLLIN) {
        ZData multipart;
        if (multipart.recv(*socket_, ZMQ_NOBLOCK)) {

          bool valid_msg = true;
          // If SUB, check topic match (implicit in ZMQ but good to be aware of
          // structure) If the first frame is topic, we might want to strip it
          // or pass it? The user request says "topic is socket_id", so
          // transparently, they might expect just the data. However, standard
          // ZMQ SUB behavior filters by prefix. The application "receives" the
          // whole multipart. Let's remove the topic frame if it matches our ID
          // (which it should), effectively hiding the complexity from the user
          // callback.

          if (pattern_ == Pattern::kSubscribe) {
            if (!multipart.empty()) {
              std::string topic = multipart.pop().to_string();
              // We trust ZMQ filtering, but if we want to hide it:
              // continue to extract data
            } else {
              valid_msg = false;
            }
          }

          if (valid_msg) {
            if (callback_) {
              callback_(multipart);  // pass multipart_t directly, zero-copy
            }
          }
        }
      }
    } catch (const zmq::error_t &e) {
      if (e.num() == ETERM || e.num() == ENOTSOCK) {
        logger::debug("Context/Socket terminated for {}", socket_id_);
        break;
      } else {
        logger::error("Pipeline error on {}: {}", socket_id_, e.what());
      }
    }
  }
  logger::debug("Receiver thread stopped for socket {}", socket_id_);
}

// -------------------------------------------------------------------------
// ZPipe Implementation
// -------------------------------------------------------------------------

std::shared_ptr<ZPipe> ZPipe::instance_ = nullptr;
std::mutex ZPipe::mutex_;

std::shared_ptr<ZPipe> ZPipe::instance() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!instance_) {
    instance_ = std::shared_ptr<ZPipe>(new ZPipe());
  }
  return instance_;
}

void ZPipe::destroyInstance() {
  std::shared_ptr<ZPipe> inst;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    inst = instance_;
    instance_ = nullptr;
  }

  if (inst) {
    // Closes all sockets
    std::map<std::string, std::shared_ptr<ZSocket>> temp_sockets;
    {
      std::lock_guard<std::mutex> sock_lock(inst->socket_mutex_);
      temp_sockets = inst->sockets_; // Copy to iterate safely
      inst->sockets_.clear();
    }

    for (auto &pair : temp_sockets) {
      pair.second->close();
    }

    if (inst->context_) {
      inst->context_->shutdown();
    }
  }
}

ZPipe::~ZPipe() {
  // We intentionally leak context_ here.
  // Deleting it invokes zmq_ctx_term() which blocks until all associated
  // background sockets/threads (e.g. OpenCV) terminate. This causes SIGSEGV
  // races during process exit if other threads are still alive.
  // We already called shutdown() in DestroyInstance(), and the OS will reap
  // the rest.
}

bool ZPipe::init(int io_threads) {
  if (!context_) {
    context_ = new zmq::context_t(io_threads);
    logger::debug("Created ZPipe with {} IO threads", io_threads);
    return true;
  }
  return false;
}

zmq::context_t *ZPipe::getContext() { return context_; }

bool ZPipe::registerSocket(std::shared_ptr<ZSocket> socket) {
  std::lock_guard<std::mutex> lock(socket_mutex_);
  if (sockets_.find(socket->getId()) != sockets_.end()) {
    logger::warn("Socket {} already registered", socket->getId());
    return false;
  }
  sockets_[socket->getId()] = socket;
  logger::debug("Registered socket {}", socket->getId());
  return true;
}

bool ZPipe::unregisterSocket(const std::string &socket_id) {
  std::lock_guard<std::mutex> lock(socket_mutex_);
  auto it = sockets_.find(socket_id);
  if (it != sockets_.end()) {
    sockets_.erase(it);
    logger::debug("Unregistered socket {}", socket_id);
    return true;
  }
  return false;
}

std::shared_ptr<ZSocket> ZPipe::getSocket(const std::string &socket_id) {
  std::lock_guard<std::mutex> lock(socket_mutex_);
  auto it = sockets_.find(socket_id);
  if (it != sockets_.end()) {
    return it->second;
  }
  return nullptr;
}

// Global helper functions
std::shared_ptr<ZPipe> createPipe(int io_threads) {
  auto pipe = ZPipe::instance();
  pipe->init(io_threads);
  return pipe;
}

void destroyPipe() { ZPipe::destroyInstance(); }

std::shared_ptr<ZSocket> getSocket(const std::string &socket_id) {
  auto pipe = ZPipe::instance();
  return pipe->getSocket(socket_id);
}

} // namespace pipe
} // namespace flame