/**
 * @file zpipe.cc
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief ZMQ Pipeline Module (ZPipe) Implementation
 */

#include "zpipe.hpp"
#include <iostream>
#include <algorithm>
#include <zmq_addon.hpp>

namespace flame {
namespace pipe {

    // -------------------------------------------------------------------------
    // Helper
    // -------------------------------------------------------------------------
    std::string pattern_to_string(Pattern p) {
        switch(p) {
            case Pattern::PUBLISH: return "publish";
            case Pattern::SUBSCRIBE: return "subscribe";
            case Pattern::PUSH: return "push";
            case Pattern::PULL: return "pull";
            case Pattern::DEALER: return "dealer";
            case Pattern::ROUTER: return "router";
            case Pattern::SERVER_PAIR: return "server_pair";
            case Pattern::CLIENT_PAIR: return "client_pair";
            default: return "unknown";
        }
    }

    // -------------------------------------------------------------------------
    // AsyncZSocket Implementation
    // -------------------------------------------------------------------------

    AsyncZSocket::AsyncZSocket(const std::string& socket_id, Pattern pattern)
        : _socket_id(socket_id), _pattern(pattern), _socket(nullptr), 
          _is_server(false), _is_created(false), _is_joined(false), 
          _worker_thread(nullptr), _stop_event(false), _callback(nullptr) {
    }

    AsyncZSocket::~AsyncZSocket() {
        close();
    }

    bool AsyncZSocket::create(std::shared_ptr<ZPipe> pipeline) {
        if (_is_created) {
            logger::warn("Socket {} already created", _socket_id);
            return true;
        }

        try {
            auto context = pipeline->get_context();
            if(!context) {
                logger::error("ZPipe context is null");
                return false;
            }

            switch(_pattern) {
                case Pattern::PUBLISH: _socket = std::make_shared<zmq::socket_t>(*context, zmq::socket_type::pub); break;
                case Pattern::SUBSCRIBE: _socket = std::make_shared<zmq::socket_t>(*context, zmq::socket_type::sub); break;
                case Pattern::PUSH: _socket = std::make_shared<zmq::socket_t>(*context, zmq::socket_type::push); break;
                case Pattern::PULL: _socket = std::make_shared<zmq::socket_t>(*context, zmq::socket_type::pull); break;
                case Pattern::ROUTER: _socket = std::make_shared<zmq::socket_t>(*context, zmq::socket_type::router); break;
                case Pattern::DEALER: _socket = std::make_shared<zmq::socket_t>(*context, zmq::socket_type::dealer); break;
                case Pattern::SERVER_PAIR: 
                case Pattern::CLIENT_PAIR: _socket = std::make_shared<zmq::socket_t>(*context, zmq::socket_type::pair); break;
            }

            // Configure socket options
            _socket->set(zmq::sockopt::linger, 0);

            if (_pattern == Pattern::SUBSCRIBE || _pattern == Pattern::PULL || _pattern == Pattern::DEALER || _pattern == Pattern::SERVER_PAIR) {
                _socket->set(zmq::sockopt::rcvhwm, 100);
                _socket->set(zmq::sockopt::rcvtimeo, 100);
                _socket->set(zmq::sockopt::reconnect_ivl, 500);
            }

            if (_pattern == Pattern::PUBLISH || _pattern == Pattern::PUSH || _pattern == Pattern::ROUTER || _pattern == Pattern::CLIENT_PAIR) {
                _socket->set(zmq::sockopt::sndhwm, 100);
                _socket->set(zmq::sockopt::sndtimeo, 100);
                _socket->set(zmq::sockopt::reconnect_ivl, 500);
            }

            pipeline->register_socket(shared_from_this());
            
            _is_created = true;
            logger::debug("Created socket {} with pattern {}", _socket_id, pattern_to_string(_pattern));
            return true;
        }
        catch (const zmq::error_t& e) {
            logger::error("Failed to create socket {}: {}", _socket_id, e.what());
            return false;
        }
    }

    bool AsyncZSocket::join(const std::string& transport, const std::string& address, int port) {
        if (!_is_created || !_socket) {
            logger::error("Socket {} not created", _socket_id);
            return false;
        }

        if (_is_joined) {
            logger::warn("Socket {} already joined", _socket_id);
            return true;
        }

        try {
            std::string conn_str;
            if (transport == "inproc" || transport == "ipc") {
                conn_str = transport + "://" + address;
            } else {
                conn_str = transport + "://" + address + ":" + std::to_string(port);
            }

            if (_pattern == Pattern::PUBLISH || _pattern == Pattern::PULL || _pattern == Pattern::ROUTER || _pattern == Pattern::SERVER_PAIR) {
                _is_server = true;
                _socket->bind(conn_str);
                logger::debug("Socket {} ({}) bound to: {}", _socket_id, pattern_to_string(_pattern), conn_str);
            } else {
                _is_server = false;
                _socket->connect(conn_str);
                logger::debug("Socket {} ({}) connected to: {}", _socket_id, pattern_to_string(_pattern), conn_str);
            }

            _is_joined = true;

            if ((_pattern == Pattern::SUBSCRIBE || _pattern == Pattern::PULL || _pattern == Pattern::ROUTER || _pattern == Pattern::DEALER || 
                 _pattern == Pattern::CLIENT_PAIR || _pattern == Pattern::SERVER_PAIR) && _callback) {
                _start_receiver_thread();
            }

            return true;

        } catch (const zmq::error_t& e) {
            logger::error("Failed to join socket {}: {}", _socket_id, e.what());
            return false;
        }
    }

    void AsyncZSocket::close() {
        if (_stop_event) {
            _stop_event = true;
        }
        if (_worker_thread && _worker_thread->joinable()) {
            _worker_thread->join();
            delete _worker_thread;
            _worker_thread = nullptr;
        }

        if (_socket) {
            _socket->close();
            _socket = nullptr; 
        }

        // Unregister logic would go here if we had back-reference to pipeline
        
        _is_created = false;
        _is_joined = false;
        logger::debug("Destroyed socket {}", _socket_id);
    }

    bool AsyncZSocket::set_message_callback(callback_t callback) {
        _callback = callback;
        return true;
    }

    bool AsyncZSocket::dispatch(const std::vector<std::string>& data) {
        if (!_socket || !_is_joined) {
            logger::error("Socket not joined");
            return false;
        }

        try {
            zmq::multipart_t multipart;
            for(const auto& s : data) {
                multipart.add(zmq::message_t(s.begin(), s.end()));
            }
            multipart.send(*_socket);
            return true;
        } catch (const zmq::error_t& e) {
            if (e.num() == EAGAIN) {
                logger::warn("Send would block");
            } else {
                logger::error("Failed to dispatch data: {}", e.what());
            }
            return false;
        }
    }

    bool AsyncZSocket::subscribe(const std::string& topic) {
        if (_pattern != Pattern::SUBSCRIBE) {
            logger::error("Subscribe can only be called on subscribe pattern");
            return false;
        }
        try {
            _socket->set(zmq::sockopt::subscribe, topic);
            logger::debug("Subscribed to topic: {}", topic);
            
            if (_is_joined && _callback) {
                _start_receiver_thread();
            }
            return true;
        } catch (const zmq::error_t& e) {
            logger::error("Failed to subscribe: {}", e.what());
            return false;
        }
    }

    bool AsyncZSocket::unsubscribe(const std::string& topic) {
        if (_pattern != Pattern::SUBSCRIBE) {
             logger::error("Unsubscribe can only be called on subscribe pattern");
            return false;
        }
        try {
            _socket->set(zmq::sockopt::unsubscribe, topic);
            logger::debug("Unsubscribed from topic: {}", topic);
            return true;
        } catch (const zmq::error_t& e) {
            logger::error("Failed to unsubscribe: {}", e.what());
            return false;
        }
    }

    void AsyncZSocket::_start_receiver_thread() {
        if (_worker_thread) return;

        _stop_event = false;
        _worker_thread = new std::thread(&AsyncZSocket::_receiver_worker, this);
        logger::debug("Receiver thread started for socket {}", _socket_id);
    }

    void AsyncZSocket::_receiver_worker() {
        while (!_stop_event) {
            try {
                zmq::pollitem_t items[] = { { static_cast<void*>(*_socket), 0, ZMQ_POLLIN, 0 } };
                zmq::poll(&items[0], 1, std::chrono::milliseconds(1000)); // 1 sec timeout

                if (items[0].revents & ZMQ_POLLIN) {
                    zmq::multipart_t multipart;
                    if(multipart.recv(*_socket, ZMQ_NOBLOCK)) {
                        std::vector<std::string> data;
                        while(!multipart.empty()){
                            data.push_back(multipart.pop().to_string());
                        }
                        if (_callback) {
                            _callback(data);
                        }
                    }
                }
            }
            catch (const zmq::error_t& e) {
                if(e.num() == ETERM) {
                    logger::debug("Context terminated");
                    break;
                }
            }
        }
        logger::debug("Receiver thread stopped for socket {}", _socket_id);
    }

    // -------------------------------------------------------------------------
    // ZPipe Implementation
    // -------------------------------------------------------------------------

    std::shared_ptr<ZPipe> ZPipe::_instance = nullptr;
    std::mutex ZPipe::_mutex;

    std::shared_ptr<ZPipe> ZPipe::get_instance() {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_instance) {
            _instance = std::shared_ptr<ZPipe>(new ZPipe());
        }
        return _instance;
    }

    void ZPipe::destroy_instance() {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_instance) {
            // Closes all sockets
            std::map<std::string, std::shared_ptr<AsyncZSocket>> temp_sockets;
            {
                std::lock_guard<std::mutex> sock_lock(_instance->_socket_mutex);
                temp_sockets = _instance->_sockets; // Copy to iterate safely
            }
            
            for(auto& pair : temp_sockets) {
                pair.second->close();
            }
            
            {
                 std::lock_guard<std::mutex> sock_lock(_instance->_socket_mutex);
                 _instance->_sockets.clear();
            }
            
            if (_instance->_context) {
                _instance->_context->shutdown();
                _instance->_context->close();
                _instance->_context = nullptr;
            }
            _instance = nullptr;
        }
    }

    ZPipe::~ZPipe() {
        // cleanup if not called explicitly
        if (_context) {
            _context->close();
        }
    }

    bool ZPipe::init(int io_threads) {
        if (!_context) {
            _context = std::make_shared<zmq::context_t>(io_threads);
            logger::debug("Created ZPipe with {} IO threads", io_threads);
            return true;
        }
        return false;
    }

    std::shared_ptr<zmq::context_t> ZPipe::get_context() {
        return _context;
    }

    bool ZPipe::register_socket(std::shared_ptr<AsyncZSocket> socket) {
        std::lock_guard<std::mutex> lock(_socket_mutex);
        if (_sockets.find(socket->get_id()) != _sockets.end()) {
            logger::warn("Socket {} already registered", socket->get_id());
            return false;
        }
        _sockets[socket->get_id()] = socket;
        logger::debug("Registered socket {}", socket->get_id());
        return true;
    }

    bool ZPipe::unregister_socket(const std::string& socket_id) {
        std::lock_guard<std::mutex> lock(_socket_mutex);
        auto it = _sockets.find(socket_id);
        if (it != _sockets.end()) {
            _sockets.erase(it);
            logger::debug("Unregistered socket {}", socket_id);
            return true;
        }
        return false;
    }

    std::shared_ptr<AsyncZSocket> ZPipe::get_socket(const std::string& socket_id) {
        std::lock_guard<std::mutex> lock(_socket_mutex);
        auto it = _sockets.find(socket_id);
        if (it != _sockets.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Global helper functions
    std::shared_ptr<ZPipe> create_pipe(int io_threads) {
        auto pipe = ZPipe::get_instance();
        pipe->init(io_threads);
        return pipe;
    }

    void destroy_pipe() {
        ZPipe::destroy_instance();
    }

    std::shared_ptr<AsyncZSocket> get_socket(const std::string& socket_id) {
        auto pipe = ZPipe::get_instance();
        return pipe->get_socket(socket_id);
    }

} // namespace pipe
} // namespace flame
