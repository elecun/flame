/**
 * @file profile.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Component profile Reader
 * @version 0.1
 * @date 2023-07-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _FLAME_CORE_PROFILE_HPP_
#define _FLAME_CORE_PROFILE_HPP_

#include <3rdparty/json.hpp>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <flame/core/def.hpp>
#include <flame/log.hpp>

using namespace std;
using json = nlohmann::json;

namespace flame::core {

    namespace task { 
        class runnable; 
        class driver;
    }

    class profile {

        friend class flame::core::task::driver;

        public:
            profile(const char* path);
            virtual ~profile() {
                data.clear();
            }

            bool is_valid() const { return valid; }

            /**
             * @brief Get the dumped JSON profile
             * 
             * @param key find by key
             * @return string dumped string
             */
            string get_dumped(const char* key = nullptr) const {
                if(key){
                    if(data.find(key)!=data.end()){
                        return data[key].dump();
                    }
                    else {
                        console::warn("{} key cannot be found.", key);
                    }
                }
                else {
                    return data.dump();
                }
                return string("{}");
            }

            const json& raw() const {
                return data;
            }
            

        protected:
            json data;

        private:
            bool valid = true;

    }; //class profile interface

}   //namespace flame

#endif