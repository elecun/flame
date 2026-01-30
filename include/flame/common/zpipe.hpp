/**
 * @file zpipe.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief ZMQ Pipeline Module (ZPipe)
 */

#ifndef FLAME_PIPE_ZPIPE_HPP_INCLUDED
#define FLAME_PIPE_ZPIPE_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <memory>
#include <zmq.hpp>
#include <flame/log.hpp>

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

    enum class Transport {
        TCP,
        INPROC,
        IPC,
        PGM,
        EPGM
    };

    std::string transport2str(Transport t);

    class AsyncZSocket : public std::enable_shared_from_this<AsyncZSocket> {
    public:
        AsyncZSocket(const std::string& socket_id, Pattern pattern);
        virtual ~AsyncZSocket();

        bool create(std::shared_ptr<ZPipe> pipeline);
        bool join(Transport transport, const std::string& address = "localhost", int port = 5555);
        void close();

        // Callback for receiving multipart data
        typedef std::function<void(const std::vector<std::string>&)> callback_t;
        bool set_message_callback(callback_t callback);

        // Send multipart data
        bool dispatch(const std::vector<std::string>& data);


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
        std::thread* _worker_thread;
        std::atomic<bool> _stop_event;
        callback_t _callback;
    };

    class ZPipe : public std::enable_shared_from_this<ZPipe> {
    public:
        static std::shared_ptr<ZPipe> get_instance();
        static void destroy_instance();

        // Initialize with io_threads
        bool init(int io_threads = 1);
        
        std::shared_ptr<zmq::context_t> get_context();

        bool register_socket(std::shared_ptr<AsyncZSocket> socket);
        bool unregister_socket(const std::string& socket_id);
        std::shared_ptr<AsyncZSocket> get_socket(const std::string& socket_id);
        
        ~ZPipe();

    private:
        ZPipe() = default;
        ZPipe(const ZPipe&) = delete;
        ZPipe& operator=(const ZPipe&) = delete;

    private:
        static std::shared_ptr<ZPipe> _instance;
        static std::mutex _mutex;

        std::shared_ptr<zmq::context_t> _context;
        std::map<std::string, std::shared_ptr<AsyncZSocket>> _sockets;
        std::mutex _socket_mutex;
    };

    // Global helper functions
    std::shared_ptr<ZPipe> create_pipe(int io_threads = 1);
    void destroy_pipe();
    std::shared_ptr<AsyncZSocket> get_socket(const std::string& socket_id);

} // namespace pipe
} // namespace flame

#endif
