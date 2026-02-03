/**
 * @file manager.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Flame Service Manager
 * @version 0.1
 * @date 2024-06-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef FLAME_MANAGER_HPP_INCLUDED
#define FLAME_MANAGER_HPP_INCLUDED

#include <dep/json.hpp>
#include <filesystem>
#include <flame/arch/singleton.hpp>
#include <flame/component/driver.hpp>
#include <flame/component/port.hpp>
#include <flame/util/uuid.hpp>
#include <map>
#include <tuple>
#include <vector>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

namespace flame {

class bundle_manager : public flame::arch::singleton<bundle_manager> {
public:
  typedef map<util::uuid_t, flame::component::driver *> bundle_container_t;

  /**
   * @brief Construct a new bundle manager object
   *
   */
  bundle_manager();

  /**
   * @brief Destroy the bundle manager object
   *
   */
  virtual ~bundle_manager();

  /**
   * @brief Install the bundle from the repository
   *
   * @param bundle_repo Path to the bundle repository
   * @return true Success
   * @return false Fail
   */
  bool install(fs::path bundle_repo);

  /**
   * @brief Uninstall the component
   *
   * @param component_name Component name to uninstall
   */
  void uninstall(const char *component_name = nullptr);

  /**
   * @brief Start the bundle service
   *
   */
  void start_bundle_service();

  /**
   * @brief Get the component count object
   *
   * @return int Number of components
   */
  int get_component_count() const { return _bundle_container.size(); }

  /**
   * @brief Get the component list object
   *
   * @return vector<string> List of component names
   */
  vector<string> get_component_list();

  /**
   * @brief Get the component info object
   *
   * @return json Component information
   */
  json get_component_info();

private:
  /**
   * @brief Bundle container
   *
   */
  bundle_container_t _bundle_container;

  /**
   * @brief Component Name-UUID Map
   *
   */
  unordered_map<string, util::uuid_t> _component_uid_map;

  /**
   * @brief UUID Generator
   *
   */
  util::uuid_generator _uuid_gen;

}; /* class */

} // namespace flame

#define manager flame::bundle_manager::instance()

#endif