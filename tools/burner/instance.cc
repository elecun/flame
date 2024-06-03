
/**
 * @file instance.cc
 * @author Byunghun hwang <bh.hwang@iae.re.kr>
 * @brief Application instance for burner
 * @version 0.1
 * @date 2024-05-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <filesystem>
#include <fstream>
#include <thread>
#include <csignal>

#include <dep/json.hpp>
#include <dep/libzmq/zmq.hpp>
#include <flame/interface/log.hpp>

#include "instance.hpp"
#include "def.hpp"
#include "manager.hpp"

using namespace std;
using json = nlohmann::json;

namespace flame_rt {

    void terminate(){
        flame_rt::cleanup();
        exit(EXIT_SUCCESS);
    }

    /* signal callback functions */
    void signal_callback(int sig) {
        switch(sig){
            case SIGSEGV: { console::warn("Signal : Segmentation violation"); } break;
            case SIGABRT: { /*console::warn("Signal : Abnormal termination");*/ } break;
            case SIGKILL: { console::warn("Signal : Process killed"); } break;
            case SIGBUS: { console::warn("Signal : Bus Error"); } break;
            case SIGTERM: { console::warn("Signal : Termination requested"); } break;
            case SIGINT: { console::warn("Signal : Interrupted"); } break;
            default:
            console::info("Cleaning up the program");
        }
        terminate(); 
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
            _cli = new remote_cli("tcp", access_port);
            console::info("Start supporting CLI...");
        }

        return true;
    }

    void run(){

    }

    void cleanup(){
        /* clear cli*/
        if(_cli)
            delete _cli;
    }
}