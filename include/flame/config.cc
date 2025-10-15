
#include "config.hpp"
#include <flame/log.hpp>
#include <flame/def.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace flame {
    config_loader::config_loader(){

    }

    config_loader::config_loader(const char* config_filepath){

        _config_abs_path = fs::canonical(fs::path(config_filepath));

        if(!load(_config_abs_path)){
            throw std::runtime_error("Configuration file load failed");
        }
    }

    config_loader::~config_loader(){
        this->_config.clear();
    }

    bool config_loader::load(const char* config_filepath){
        _config_abs_path = fs::canonical(fs::path(config_filepath));
        if(!load(_config_abs_path)){
            throw std::runtime_error("Configuration file load failed");
            return false;
        }
        return true;
    }

    bool config_loader::is_loaded(){
        if(!_config.empty())
            return true;
        return false;
    }

    bool config_loader::load(filesystem::path config_filepath){
        return __load(config_filepath);
        return true;
    }

    bool config_loader::__load(fs::path filepath){
    
        try {

            /* file existance check */
            if(!filesystem::exists(filepath)){
                logger::error("Configuration file does not exist.");
                return false;
            }

            /* load configurations from file*/
            std::ifstream file(filepath.string());
            file >> this->_config;
        }
        catch(const json::exception& e){
            logger::error("configuration file load failed : {}", e.what());
            return false;
        }
        catch(const std::ifstream::failure& e){
            logger::error("configuration file load failed : {}", e.what());
            return false;
        }
        catch(const filesystem::filesystem_error& e){
            logger::error("configuration file load failed : {}", e.what());
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

    string config_loader::get_bundle_name() {
        if(!_config.empty()){
            if(_config.contains(def::BUNDLE)){
                if(_config[def::BUNDLE].contains(def::BUNDLE_NAME)){
                    string name = _config[def::BUNDLE][def::BUNDLE_NAME].get<string>();
                    return name;
                }
            }
        }
        return string("");
    }

    filesystem::path config_loader::get_bundle_path() const{
        if(!_config.empty()){
            filesystem::path _conf_path(_config_abs_path);
            filesystem::path _bundle_path = filesystem::canonical(_conf_path).parent_path() / filesystem::path(def::BUNDLE);
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
            json _topology = _config[def::BUNDLE][def::BUNDLE_TOPOLOGY];
            if(_topology.contains("data")){
                for(const auto& con: _topology){
                    _topology_map.insert(make_pair(con.at("provided").get<string>(), con.at("required").get<string>()));
                }

                return _topology_map;
            }
            else {
                logger::info("Not defined data port connections");
            }

        }
        catch(json::exception& e){
            logger::warn("Exception for reading dataport tolpology", e.what());
        }

        return map<string, string>(); //empty
    }

    map<string, string> config_loader::get_service_topology(){
        return map<string, string>(); //empty
    }

    json config_loader::get_parameters(){
        return json::object();
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
    //             logger::info("Totally installed : {}", manager->size());
    //         }
    //     }

    //     }

    //     return false;
    // }
}