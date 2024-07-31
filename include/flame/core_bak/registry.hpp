/**
 * @file registry.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Memory Registry
 * @version 0.1
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _FLAME_CORE_REGISTRY_HPP_
#define _FLAME_CORE_REGISTRY_HPP_

#include <any>
#include <string>
#include <unordered_map>
#include <flame/arch/singleton.hpp>
#include <flame/log.hpp>

using namespace std;
using namespace flame;

namespace flame {
    
    class registry : public flame::arch::singleton<registry> {
        public:
            registry() = default;
            virtual ~registry() = default;

            virtual bool insert(const string key, const any value){
                if(registry_container.find(key)==registry_container.end()){
                    registry_container.insert(std::pair<string, any>(key, value));
                    return true;
                }
                else {
                    logger::warn("{} has already existed in the system registry.", key);
                    return false;
                }
            }

            template<typename _Type>
            _Type get(const string key){
                if(registry_container.find(key)!=registry_container.end())
                    return std::any_cast<_Type>(registry_container[key]);
                else
                    return nullptr;
            }

            bool find(const char* key){
                if(registry_container.find(key)==registry_container.end())
                    return false;
                return true;
            }

        private:
            unordered_map<string, any> registry_container;
    };
} //namespace

#define registry flame::registry::instance()

#endif