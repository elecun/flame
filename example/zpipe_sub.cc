/**
 * @file zpipe_sub.cc
 * @brief ZPipe Subscriber Example
 */

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <flame/common/zpipe.hpp>

// Callback function
void on_message(const std::vector<std::string>& data) {
    std::cout << "Received: ";
    for (const auto& part : data) {
        std::cout << "[" << part << "] ";
    }
    std::cout << std::endl;
}

int main() {
    
    // 1. Create Pipe
    auto pipe = flame::pipe::create_pipe(1);

    // 2. Create Subscriber Socket
    // Socket ID must match the Publisher's Socket ID for implicit topic subscription
    auto sub_socket = std::make_shared<flame::pipe::AsyncZSocket>("pub1", flame::pipe::Pattern::SUBSCRIBE);
    if (!sub_socket->create(pipe)) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // 3. Set Callback
    sub_socket->set_message_callback(on_message);

    // 4. Connect
    if (!sub_socket->join(flame::pipe::Transport::TCP, "localhost", 5555)) {
        std::cerr << "Failed to join" << std::endl;
        return -1;
    }

    // 5. Subscribe
    // Indirectly subscribed to "pub1" because of socket_id
    std::cout << "Subscriber started on tcp://localhost:5555, topic: pub1" << std::endl;

    // 6. Wait (Callback handles data)
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    flame::pipe::destroy_pipe();
    return 0;
}
