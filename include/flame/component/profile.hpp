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
#include <flame/def.hpp>

using namespace std;
using namespace flame;
namespace fs = std::filesystem;
using json = nlohmann::json;


namespace flame::component {
    class Driver;
    class Profile final{

        friend class flame::component::Driver;
        public:
            Profile(fs::path profile_path){ // profile_path = *.profile absolute path
                try{
                    std::ifstream file(profile_path.string().c_str());
                    file >> raw_profile_;
                    
                }
                catch(const json::exception& e){
                    raw_profile_.clear();
                    throw std::runtime_error(e.what());
                }
            }
            
            virtual ~Profile() {
                raw_profile_.clear();
            }

            /**
             * @brief Get the dumped JSON profile
             * 
             * @param key find by key
             * @return string dumped string
             */
            string getDumped(const char* key = nullptr) const {
                if(key){
                    if(raw_profile_.contains(key)){
                        return raw_profile_[key].dump();
                    }
                    else {
                        logger::warn("{} key cannot be found.", key);
                    }
                }
                else {
                    return raw_profile_.dump();
                }
                return string("{}");
            }

            const json& raw() const {
                return raw_profile_;
            }

            /**
             * @brief parameter section data in profile
             * 
             * @return json 
             */
            json parameters() {
                try{
                    if(raw_profile_.contains(def::kProfileParameters)){
                        return raw_profile_[def::kProfileParameters];
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
            json dataPort() {
                try {
                    if(raw_profile_.contains(def::kProfileDataport)){
                        return raw_profile_[def::kProfileDataport];
                    }
                    return json::object();
                }
                catch(const json::exception& e){
                    throw std::runtime_error(e.what());
                }
                return json::object();
            }
            

        protected:
            json raw_profile_;

    }; //class Profile interface

} /* namespace */
 
#endif