/**
 * @file zpipe_pull.cc
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Pull Example based Zpipe
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
    std::cout << "Pulled: " << data.popstr() << std::endl;
}

int main() {
    
    // 1. Create Pipe
    auto pipe = flame::pipe::createPipe(1);

    // 2. Create Pull Socket
    auto pull_socket = std::make_shared<flame::pipe::ZSocket>("puller1", flame::pipe::Pattern::Pull);
    if (!pull_socket->create(pipe)) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // 3. Set Callback
    pull_socket->setMessageCallback(on_message);

    // 4. Connect
    if (!pull_socket->join(flame::pipe::Transport::Tcp, "localhost", 5556)) {
        std::cerr << "Failed to join" << std::endl;
        return -1;
    }

    std::cout << "Puller started on tcp://localhost:5556" << std::endl;

    // 5. Wait (Callback handles data)
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    flame::pipe::destroyPipe();
    return 0;
}
