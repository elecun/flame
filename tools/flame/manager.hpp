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
#include <unordered_map>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

namespace flame {

class BundleManager : public flame::arch::Singleton<BundleManager> {
public:
  typedef map<util::UuidT, flame::component::Driver *> BundleContainerT;

  /**
   * @brief Construct a new bundle manager object
   *
   */
  BundleManager();

  /**
   * @brief Destroy the bundle manager object
   *
   */
  virtual ~BundleManager();

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
  void startBundleService();

  /**
   * @brief Get the component count object
   *
   * @return int Number of components
   */
  int getComponentCount() const { return bundle_container_.size(); }

  /**
   * @brief Get the component list object
   *
   * @return vector<string> List of component names
   */
  vector<string> getComponentList();

  /**
   * @brief Get the component info object
   *
   * @return json Component information
   */
  json getComponentInfo();

private:
  /**
   * @brief Bundle container
   *
   */
  BundleContainerT bundle_container_;

  /**
   * @brief Component Name-UUID Map
   *
   */
  unordered_map<string, util::UuidT> component_uid_map_;

  /**
   * @brief UUID Generator
   *
   */
  util::UuidGenerator uuid_gen_;

}; /* class */

} // namespace flame

#define MANAGER flame::BundleManager::instance()

#endif