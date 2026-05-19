/**
 * @file zpipe_sub.cc
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Subscriber Example based Zpipe
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

// Callback function
void on_message(flame::pipe::ZData& data) {
    std::cout << "Received: " << data.popstr() << std::endl;
}

int main() {
    
    // 1. Create Pipe
    auto pipe = flame::pipe::createPipe(1);

    // 2. Create Subscriber Socket
    // Socket ID must match the Publisher's Socket ID for implicit topic subscription
    auto sub_socket = std::make_shared<flame::pipe::ZSocket>("topic1", flame::pipe::Pattern::Subscribe);
    if (!sub_socket->create(pipe)) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // 3. Set Callback
    sub_socket->setMessageCallback(on_message);

    // 4. Connect
    if (!sub_socket->join(flame::pipe::Transport::Tcp, "localhost", 5555)) {
        std::cerr << "Failed to join" << std::endl;
        return -1;
    }

    // 5. Subscribe
    // Indirectly subscribed to "topic1" because of socket_id
    std::cout << "Subscriber started on tcp://localhost:5555, topic: topic1" << std::endl;

    // 6. Wait (Callback handles data)
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    flame::pipe::destroyPipe();
    return 0;
}
