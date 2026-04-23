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
#include <flame/arch/singleton.hpp>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

namespace flame {

    class ConfigLoader : public flame::arch::Singleton<ConfigLoader>{
        public:
            ConfigLoader(const char* config_filepath);
            ConfigLoader();
            virtual ~ConfigLoader();

            /**
             * @brief load configuration file
             * 
             * @param config_filepath configuration file path
             * @return true if success
             * @return false if load failed
             */
            bool load(const char* config_filepath);
            bool load(filesystem::path config_filepath);

            bool isLoaded();


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
            json getConfig() const {
                return config_;
            }

            /**
             * @brief Get the path object
             * 
             * @return const char* return configuration file path
             */
            filesystem::path getConfigPath() const {
                return config_abs_path_;
            }

            map<string, string> getDataTopology();
            map<string, string> getServiceTopology();

            filesystem::path getBundlePath() const;
            string getBundleName();

            json getParameters();

        private:
            bool loadInternal(fs::path filepath);

        private:
            fs::path config_abs_path_;
            json config_;

    }; /* class */

} /* namespace */

#define CONFIG flame::ConfigLoader::instance()


#endif