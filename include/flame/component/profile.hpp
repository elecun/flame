/**
 * @file profile.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component profile
 * @version 0.1
 * @date 2024-06-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_COMPONENT_PROFILE_HPP_INCLUDED
#define FLAME_COMPONENT_PROFILE_HPP_INCLUDED

#include <dep/json.hpp>
#include <filesystem>
#include <flame/log.hpp>
#include <fstream>
#include <flame/config_def.hpp>

using namespace std;
using namespace flame;
namespace fs = std::filesystem;
using json = nlohmann::json;


namespace flame::component {
    class driver;
    class profile final{

        friend class flame::component::driver;
        public:
            profile(fs::path profile_path){ // profile_path = *.profile absolute path
                try{
                    std::ifstream file(profile_path.string().c_str());
                    file >> raw_profile;
                    
                }
                catch(const json::exception& e){
                    raw_profile.clear();
                    throw std::runtime_error(e.what());
                }
            }
            
            virtual ~profile() {
                raw_profile.clear();
            }

            /**
             * @brief Get the dumped JSON profile
             * 
             * @param key find by key
             * @return string dumped string
             */
            string get_dumped(const char* key = nullptr) const {
                if(key){
                    if(raw_profile.contains(key)){
                        return raw_profile[key].dump();
                    }
                    else {
                        console::warn("{} key cannot be found.", key);
                    }
                }
                else {
                    return raw_profile.dump();
                }
                return string("{}");
            }

            const json& raw() const {
                return raw_profile;
            }

            /**
             * @brief parameter section data in profile
             * 
             * @return json 
             */
            json parameters() {
                try{
                    if(raw_profile.contains(__PROFILE_PARAMETERS__)){
                        return raw_profile[__PROFILE_PARAMETERS__];
                    }
                    return json::object();

                }
                catch(const json::exception& e){
                    throw std::runtime_error(e.what());
                }

                return json::object();
            }

            /**
             * @brief dataport section data in profile
             * 
             * @return json 
             */
            json dataport() {
                try {
                    if(raw_profile.contains(__PROFILE_DATAPORT__)){
                        return raw_profile[__PROFILE_DATAPORT__];
                    }
                    return json::object();
                }
                catch(const json::exception& e){
                    throw std::runtime_error(e.what());
                }
                return json::object();
            }
            

        protected:
            json raw_profile;

    }; //class profile interface

} /* namespace */
 
#endif