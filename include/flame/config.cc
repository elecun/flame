
#include "config.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace flame {
    config_loader::config_loader(const char* config_filepath){

        _config_abs_path = fs::canonical(fs::path(config_filepath));
        // console::info("Configuration File : {}", _config_abs_path.string());

        if(!load(_config_abs_path)){
            throw std::runtime_error("Configuration file load failed");
        }
    }

    config_loader::~config_loader(){
        this->_config.clear();
    }

    bool config_loader::load(const char* config_filepath){
        // filesystem::path _conf_path(config_filepath);
        // return __load(_conf_path);
        return true;
    }

    bool config_loader::load(filesystem::path config_filepath){
        return __load(config_filepath);
        return true;
    }

    bool config_loader::__load(fs::path filepath){
    
        try {

            /* file existance check */
            if(!filesystem::exists(filepath)){
                console::error("Configuration file does not exist.");
                return false;
            }

            /* load configurations from file*/
            std::ifstream file(filepath.string());
            file >> this->_config;
        }
        catch(const json::exception& e){
            console::error("configuration file load failed : {}", e.what());
            return false;
        }
        catch(const std::ifstream::failure& e){
            console::error("configuration file load failed : {}", e.what());
            return false;
        }
        catch(const filesystem::filesystem_error& e){
            console::error("configuration file load failed : {}", e.what());
            return false;
        }

        return true;

    }

    bool config_loader::reload(){
        this->_config.clear();
        this->load(this->_config_abs_path);
        return false;
    }

    bool config_loader::exist(initializer_list<string> keys){
        return true;
    }

    const char* config_loader::get_bundle_name() const{
        if(!_config.empty()){
            if(_config.contains(__CONFIG_KEY_BUNDLE__)){
                if(_config[__CONFIG_KEY_BUNDLE__].contains(__CONFIG_KEY_BUNDLE_NAME__)){
                    string name = _config[__CONFIG_KEY_BUNDLE__][__CONFIG_KEY_BUNDLE_NAME__].get<string>();
                    console::info("found bundle : {}", name);
                    return name.c_str();
                }
            }
        }
        return nullptr;
    }

    filesystem::path config_loader::get_bundle_path() const{
        if(!_config.empty()){
            filesystem::path _conf_path(_config_abs_path);
            filesystem::path _bundle_path = filesystem::canonical(_conf_path).parent_path() / filesystem::path(__CONFIG_KEY_BUNDLE__);
            // console::info("path = {}", _bundle_path.string());
            return _bundle_path;
        }
        else{
            throw std::runtime_error("Configuration must be loaded");
        }

        return string("");
    }

    map<string, string> config_loader::get_data_topology(){
        map<string, string> _topology_map;
        try{
            json _topology = _config[__CONFIG_KEY_BUNDLE__][__CONFIG_KEY_BUNDLE_TOPOLOGY__];
            if(_topology.contains("data")){
                for(const auto& con: _topology){
                    _topology_map.insert(make_pair(con.at("provided").get<string>(), con.at("required").get<string>()));
                }

                return _topology_map;
            }
            else {
                console::info("Not defined data port connections");
            }

        }
        catch(json::exception& e){
            console::warn("Exception for reading dataport tolpology", e.what());
        }

        return map<string, string>(); //empty
    }

    map<string, string> config_loader::get_service_topology(){
        return map<string, string>(); //empty
    }

    // bool config_loader::exist(const char* key){
    //     if(_config){
    //         if(_config.find(key)!=_config.end())
    //             return true;

    //         if(config.find(_CONFIG_REQ_KEY_)!=config.end()){

    //         /* components */
    //         if(config[_CONFIG_REQ_KEY_].find(_CONFIG_COMPONENTS_KEY_)!=config[_CONFIG_REQ_KEY_].end()){
    //             vector<string> required_components = config[_CONFIG_REQ_KEY_][_CONFIG_COMPONENTS_KEY_].get<std::vector<string>>();
    //             for(string& component:required_components){
    //                 manager->install(component.c_str());
    //             }
    //             console::info("Totally installed : {}", manager->size());
    //         }
    //     }

    //     }

    //     return false;
    // }
}