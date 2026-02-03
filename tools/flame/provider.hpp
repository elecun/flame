/**
 * @file provider.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief State Provider for Flame Instance
 * @version 0.1
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef FLAME_PROVIDER_HPP_INCLUDED
#define FLAME_PROVIDER_HPP_INCLUDED

#include <atomic>
#include <flame/common/zpipe.hpp>
#include <memory>
#include <string>
#include <thread>

namespace flame {

class StateProvider {
public:
  /**
   * @brief Construct a new State Provider object
   *
   */
  StateProvider();

  /**
   * @brief Destroy the State Provider object
   *
   */
  virtual ~StateProvider();

  /**
   * @brief Start the state provider service
   *
   */
  void start();

  /**
   * @brief Connect to the flame instance
   *
   */
  void connect();

  /**
   * @brief Stop the service
   *
   */
  void stop();

private:
  /**
   * @brief Publish loop
   *
   */
  void _publish_loop();

private:
  /**
   * @brief Service running flag
   *
   */
  std::atomic<bool> _run_service;

  /**
   * @brief Service thread
   *
   */
  std::thread *_t_service;

  /**
   * @brief IPC address
   *
   */
  std::string _ipc_addr;
};

} // namespace flame

#endif
