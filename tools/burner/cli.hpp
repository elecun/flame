/**
 * @file cli.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Remote CLi(Commnad Line Interface) with ZMQ
 * @version 0.1
 * @date 2024-05-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_BURNER_CLI_HPP_INCLUDED
#define FLAME_BURNER_CLI_HPP_INCLUDED

#include <atomic>
#include <thread>
#include <string>
#include <queue>
#include <dep/libzmq/zmq.hpp>

using namespace std;

namespace flame_rt {

class remote_cli {
    public:
    remote_cli(const char* socket_type, int access_port);
    ~remote_cli();

    private:
    /**
     * @brief command process
     * 
     */
    void process();

    private:
    zmq::context_t _context;
    zmq::socket_t _socket;
    atomic<bool> _terminate = false;
    thread _worker;
    queue<string> _req_queue;

}; /* class */

} /* namespace */

#endif