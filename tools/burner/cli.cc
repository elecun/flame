
#include "cli.hpp"
#include <flame/interface/log.hpp>

namespace flame_rt {

    remote_cli::remote_cli(const char* socket_type = "tcp", int access_port = 5555)
    :_context(1), _socket(_context, zmq::socket_type::rep), _terminate(false){
        string address = fmt::format("{}://*:{}", socket_type, access_port);
        this->_socket.bind(address);
        this->_worker = thread(&remote_cli::process, this);
        console::info("CLI Accessiable {}", address);
    }

    remote_cli::~remote_cli() {
        this->_terminate = true;
        if(this->_worker.joinable()){
            this->_worker.join();
        }
    }

    void remote_cli::process(){
        while(!_terminate){
            try {
                zmq::message_t recv_msgs;
                zmq::recv_result_t rc(this->_socket.recv(recv_msgs, zmq::recv_flags::none)); //blocked
                if(rc.has_value())
                if(rc.has_value() && (EAGAIN == rc.value())){
                    console::warn("?");
                }
                console::info("received : {}",recv_msgs.to_string());

                std::string received_msg = "reply";
                zmq::message_t reply(received_msg.size());
                memcpy(reply.data(), received_msg.data(), received_msg.size());
                this->_socket.send(reply, zmq::send_flags::dontwait);
            }
            catch(zmq::error_t& e){
                console::warn("error : {}", e.what());
            }

            
            // string received_msg = std::string(static_cast<char*>(request.data()), request.size());
            // _req_queue.push(received_msg);

            if(!_req_queue.empty()){
                string req = _req_queue.front();
                _req_queue.pop();

                console::info("cli processed : {}", req);
            }

            this_thread::sleep_for(chrono::milliseconds(100));
        }

        console::info("process out");
    }

}