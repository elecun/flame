
#include "instance.hpp"
#include <filesystem>
#include <flame/log.hpp>
#include <flame/sys/cpu.hpp>
#include <flame/util/file.hpp>
#include <flame/core/profile.hpp>
#include <flame/core/profile_def.hpp>
#include <flame/core/registry.hpp>
#include <flame/core/manager.hpp>

using namespace std;
using namespace flame;

namespace flame::app {

    /**
     * @brief Flame executor initialization
     * 
     * @param conf_file Json formmated configuration file
     * @return true if initialize success
     * @return false initialize failed
     */
    bool initialize(const char* conf_file_path){

        /* system information summary */
        console::info("Process Id = {}", getpid());
        console::info("System CPUs = {}", get_nprocs());
        console::info("System Clock Ticks = {}", sysconf(_SC_CLK_TCK));

        profile_data config;
        try {
            filesystem::path _conf_path(conf_file_path);
            console::info("Configuration file : {}", filesystem::canonical(_conf_path).string());
            console::info("Parent path : {}", filesystem::canonical(_conf_path).parent_path().string());
            if(!filesystem::exists(_conf_path)){
                console::error("Configuration file does not exist.");
                return false;
            }

            // read profile from file
            std::ifstream file(_conf_path.string());
            file >> config;
        }
        catch(const profile_data::exception& e){
            spdlog::error("Configuration file load failed : {}", e.what());
            return false;
        }
        catch(std::ifstream::failure& e){
            spdlog::error("Configuration file load failed : {}", e.what());
            return false;
        }
        catch(filesystem::filesystem_error& e){
            console::error("Configuration file load failed : {}", e.what());
            return false;
        }

        // set environments with profile
        if(config.find(_CONFIG_ENV_KEY_)!=config.end()){
            
            /* Set path (Absolute) */
            if(config[_CONFIG_ENV_KEY_].find(_CONFIG_PATH_KEY_)!=config[_CONFIG_ENV_KEY_].end()){
                for(profile_data::iterator it = config[_CONFIG_ENV_KEY_][_CONFIG_PATH_KEY_].begin(); it != config[_CONFIG_ENV_KEY_][_CONFIG_PATH_KEY_].end(); ++it){
                    if(it.value().is_string()){
                        registry->insert(it.key(), std::make_any<std::string>(config[_CONFIG_ENV_KEY_][_CONFIG_PATH_KEY_][it.key()].get<std::string>()));
                        console::info("+ Register Path : {}={}", it.key(),(registry->get<std::string>(it.key())));
                    }
                }
            }   
        }

        /* required */
        if(config.find(_CONFIG_REQ_KEY_)!=config.end()){

            /* components */
            if(config[_CONFIG_REQ_KEY_].find(_CONFIG_COMPONENTS_KEY_)!=config[_CONFIG_REQ_KEY_].end()){
                vector<string> required_components = config[_CONFIG_REQ_KEY_][_CONFIG_COMPONENTS_KEY_].get<std::vector<string>>();
                for(string& component:required_components){
                    manager->install(component.c_str());
                }
                console::info("Totally installed : {}", manager->size());
            }
        }

        return true;
    }

    void run(){
        manager->run();
        
    }

    void cleanup(){
        manager->uninstall();
    }

} /** namespace */