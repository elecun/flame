
#include "instance.hpp"
#include "def.hpp"

#include <csignal>
#include <filesystem>
#include <fstream>

#include <dep/json.hpp>
#include <dep/libzmq/zmq.hpp>
#include <flame/interface/log.hpp>


using namespace std;
using namespace flame;
using json = nlohmann::json;

namespace flame::tools {

    /**
     * @brief cleanup and termination
     * 
     */
    void cleanup(){

    }
    void cleanup_and_exit(){
        flame::tools::cleanup();
        console::info("Process will be terminated");
        exit(EXIT_SUCCESS);
    }

    /**
     * @brief signal event callback functions
     * 
     * @param sig signal number
     */
    void signal_callback(int sig) {
        switch(sig){
            case SIGSEGV: { console::warn("Segmentation violation"); } break;
            case SIGABRT: { console::warn("Abnormal termination"); } break;
            case SIGKILL: { console::warn("Process killed"); } break;
            case SIGBUS: { console::warn("Bus Error"); } break;
            case SIGTERM: { console::warn("Termination requested"); } break;
            case SIGINT: { console::warn("Interrupted"); } break;
            default:
            console::info("Cleaning up the program");
        }
        cleanup_and_exit();
    }

    bool init(const char* config_path){

        // 1. read configuration file(*.conf as JSON)
        filesystem::path _conf_path(config_path);
        json config;

        try {
            console::info("Config file path(absolute) : {}", filesystem::canonical(_conf_path).string());
            console::info("Parent path : {}", filesystem::canonical(_conf_path).parent_path().string());

            /* file existance check */
            if(!filesystem::exists(_conf_path)){
                console::error("Configuration file does not exist.");
                return false;
            }

            /* read configurations from file*/
            std::ifstream file(_conf_path.string());
            file >> config;
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

        /* zmq cli server creation */
        if(config.find(__CONFIG_KEY_ACCESS__)!=config.end()){
            int access_port = config[__CONFIG_KEY_ACCESS__].get<int>();
            // _cli = new remote_cli("tcp", access_port);
            console::info("Start supporting CLI...");
        }

        return true;


        // /* system information summary */
        // console::info("Process Id = {}", getpid());
        // console::info("System CPUs = {}", get_nprocs());
        // console::info("System Clock Ticks = {}", sysconf(_SC_CLK_TCK));

        // profile_data config;
        // try {
        //     filesystem::path _conf_path(conf_file_path);
        //     console::info("Configuration file : {}", filesystem::canonical(_conf_path).string());
        //     console::info("Parent path : {}", filesystem::canonical(_conf_path).parent_path().string());
        //     if(!filesystem::exists(_conf_path)){
        //         console::error("Configuration file does not exist.");
        //         return false;
        //     }

        //     // read profile from file
        //     std::ifstream file(_conf_path.string());
        //     file >> config;
        // }
        // catch(const profile_data::exception& e){
        //     spdlog::error("Configuration file load failed : {}", e.what());
        //     return false;
        // }
        // catch(std::ifstream::failure& e){
        //     spdlog::error("Configuration file load failed : {}", e.what());
        //     return false;
        // }
        // catch(filesystem::filesystem_error& e){
        //     console::error("Configuration file load failed : {}", e.what());
        //     return false;
        // }

        // // set environments with profile
        // if(config.find(_CONFIG_ENV_KEY_)!=config.end()){
            
        //     /* Set path (Absolute) */
        //     if(config[_CONFIG_ENV_KEY_].find(_CONFIG_PATH_KEY_)!=config[_CONFIG_ENV_KEY_].end()){
        //         for(profile_data::iterator it = config[_CONFIG_ENV_KEY_][_CONFIG_PATH_KEY_].begin(); it != config[_CONFIG_ENV_KEY_][_CONFIG_PATH_KEY_].end(); ++it){
        //             if(it.value().is_string()){
        //                 registry->insert(it.key(), std::make_any<std::string>(config[_CONFIG_ENV_KEY_][_CONFIG_PATH_KEY_][it.key()].get<std::string>()));
        //                 console::info("+ Register Path : {}={}", it.key(),(registry->get<std::string>(it.key())));
        //             }
        //         }
        //     }   
        // }

        // /* required */
        // if(config.find(_CONFIG_REQ_KEY_)!=config.end()){

        //     /* components */
        //     if(config[_CONFIG_REQ_KEY_].find(_CONFIG_COMPONENTS_KEY_)!=config[_CONFIG_REQ_KEY_].end()){
        //         vector<string> required_components = config[_CONFIG_REQ_KEY_][_CONFIG_COMPONENTS_KEY_].get<std::vector<string>>();
        //         for(string& component:required_components){
        //             manager->install(component.c_str());
        //         }
        //         console::info("Totally installed : {}", manager->size());
        //     }
        // }

        // return true;
    }

    void run_bundle(){

    }

} /** namespace */