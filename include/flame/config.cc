
#include "config.hpp"
#include <flame/log.hpp>
#include <flame/def.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace flame {
    ConfigLoader::ConfigLoader(){

    }

    ConfigLoader::ConfigLoader(const char* config_filepath){

        config_abs_path_ = fs::canonical(fs::path(config_filepath));

        if(!load(config_abs_path_)){
            throw std::runtime_error("Configuration file load failed");
        }
    }

    ConfigLoader::~ConfigLoader(){
        this->config_.clear();
    }

    bool ConfigLoader::load(const char* config_filepath){
        config_abs_path_ = fs::canonical(fs::path(config_filepath));
        if(!load(config_abs_path_)){
            throw std::runtime_error("Configuration file load failed");
            return false;
        }
        return true;
    }

    bool ConfigLoader::isLoaded(){
        if(!config_.empty())
            return true;
        return false;
    }

    bool ConfigLoader::load(filesystem::path config_filepath){
        return loadInternal(config_filepath);
    }

    bool ConfigLoader::loadInternal(fs::path filepath){
    
        try {

            /* file existance check */
            if(!filesystem::exists(filepath)){
                logger::error("Configuration file does not exist.");
                return false;
            }

            /* load configurations from file*/
            std::ifstream file(filepath.string());
            file >> this->config_;
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

    bool ConfigLoader::reload(){
        this->config_.clear();
        this->load(this->config_abs_path_);
        return false;
    }

    bool ConfigLoader::exist(initializer_list<string> keys){
        return true;
    }

    string ConfigLoader::getBundleName() {
        if(!config_.empty()){
            if(config_.contains(def::kBundle)){
                if(config_[def::kBundle].contains(def::kBundleName)){
                    string name = config_[def::kBundle][def::kBundleName].get<string>();
                    return name;
                }
            }
        }
        return string("");
    }

    filesystem::path ConfigLoader::getBundlePath() const{
        if(!config_.empty()){
            filesystem::path _conf_path(config_abs_path_);
            filesystem::path _bundle_path = filesystem::canonical(_conf_path).parent_path() / filesystem::path(def::kBundle);
            return _bundle_path;
        }
        else{
            throw std::runtime_error("Configuration must be loaded");
        }

        return string("");
    }

    map<string, string> ConfigLoader::getDataTopology(){
        map<string, string> _topology_map;
        try{
            json _topology = config_[def::kBundle][def::kBundleTopology];
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

    map<string, string> ConfigLoader::getServiceTopology(){
        return map<string, string>(); //empty
    }

    json ConfigLoader::getParameters(){
        return json::object();
    }

}