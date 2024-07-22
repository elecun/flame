/**
 * @file config.hpp
 * @author Byunghun hwang (bh.hwang@iae.re.kr)
 * @brief Configuration file parser
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_CONFIG_HPP_INCLUDED
#define FLAME_CONFIG_HPP_INCLUDED

#include <dep/json.hpp>
#include <string>
#include <initializer_list>
#include <filesystem>
#include <map>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

namespace flame {

    class config_loader {
        public:
            config_loader(const char* config_filepath);
            virtual ~config_loader();

            /**
             * @brief load configuration file
             * 
             * @param config_filepath configuration file path
             * @return true if success
             * @return false if load failed
             */
            bool load(const char* config_filepath);
            bool load(filesystem::path config_filepath);


            /**
             * @brief reload configuraion from file (useful for file changed)
             * 
             * @return true if success
             * @return false if failed
             */
            bool reload();

            /**
             * @brief find and check existance
             * 
             * @param key key to find
             * @return true if exist
             * @return false not exist
             */
            bool exist(initializer_list<string> keys);

            /**
             * @brief Get the config object
             * 
             * @return json configuration parameters (read only)
             */
            json get_config() const {
                return _config;
            }

            /**
             * @brief Get the path object
             * 
             * @return const char* return configuration file path
             */
            filesystem::path get_config_path() const {
                return _config_abs_path;
            }

            map<string, string> get_data_topology();
            map<string, string> get_service_topology();

            filesystem::path get_bundle_path() const;
            string get_bundle_name();

        private:
            bool __load(fs::path filepath);

        private:
            fs::path _config_abs_path;
            json _config;

    }; /* class */

} /* namespace */


#endif