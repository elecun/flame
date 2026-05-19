/**
 * @file zpipe_push.cc
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Push Example based Zpipe
 * @version 0.1
 * @date 2026-05-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <flame/common/zpipe.hpp>

int main() {
    
    // 1. Create Pipe
    auto pipe = flame::pipe::createPipe(1);

    // 2. Create Push Socket
    auto push_socket = std::make_shared<flame::pipe::ZSocket>("pusher1", flame::pipe::Pattern::Push);
    if (!push_socket->create(pipe)) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // 3. Bind
    if (!push_socket->join(flame::pipe::Transport::Tcp, "*", 5556)) {
        std::cerr << "Failed to join" << std::endl;
        return -1;
    }

    std::cout << "Pusher started on tcp://*:5556" << std::endl;

    // 4. Push loop
    int count = 0;
    while (true) {
        flame::pipe::ZData msg;
        msg.addstr("Task " + std::to_string(count));

        if (push_socket->dispatch(msg)) {
            std::cout << "Pushed: Task " << count << std::endl;
        } else {
            std::cerr << "Failed to push" << std::endl;
        }

        count++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    flame::pipe::destroyPipe();
    return 0;
}
