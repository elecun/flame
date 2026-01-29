/**
 * @file zpipe_pub.cc
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Publisher Example based Zpipe
 * @version 0.1
 * @date 2026-01-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <flame/pipe/zpipe.hpp>

int main() {
    
    // 1. Create Pipe
    auto pipe = flame::pipe::create_pipe(1);

    // 2. Create Publisher Socket
    auto pub_socket = std::make_shared<flame::pipe::AsyncZSocket>("pub1", flame::pipe::Pattern::PUBLISH);
    if (!pub_socket->create(pipe)) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // 3. Bind
    if (!pub_socket->join("tcp", "*", 5555)) {
        std::cerr << "Failed to join" << std::endl;
        return -1;
    }

    std::cout << "Publisher started on tcp://*:5555" << std::endl;

    // 4. Publish loop
    int count = 0;
    while (true) {
        std::vector<std::string> msg;
        msg.push_back("topicA"); // Topic
        msg.push_back("Message " + std::to_string(count));

        if (pub_socket->dispatch(msg)) {
            std::cout << "Sent: Message " << count << std::endl;
        } else {
            std::cerr << "Failed to send" << std::endl;
        }

        count++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    flame::pipe::destroy_pipe();
    return 0;
}
