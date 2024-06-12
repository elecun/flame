
#include "instance.hpp"
#include "def.hpp"

#include <csignal>
#include <filesystem>
#include <fstream>

#include <dep/json.hpp>
#include <dep/libzmq/zmq.hpp>
#include <flame/log.hpp>


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
        }

        /* bundle name */
        if(config.find(__CONFIG_KEY_BUNDLE__)!=config.end()){

        }

        return true;

    }

    void run_bundle(const char* bundle){
        if(bundle){ //bundle name was selected
            filesystem::path _bundle_path(bundle);
            if(filesystem::exists(_bundle_path)){
                if(filesystem::is_regular_file(_bundle_path)){
                    // if bundle is file(tarball-based package)
                }
                else if(filesystem::is_directory(_bundle_path)){
                    
                }
                else {
                    console::warn("No bundle to perform.");
                }
            }


            filesystem::canonical(_bundle_path).parent_path().string();
            console::info("Parent path : {}", filesystem::canonical(_bundle_path).parent_path().string());
        }
        else {
            
        }
    }

} /** namespace */