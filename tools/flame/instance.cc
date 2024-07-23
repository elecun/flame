
#include "instance.hpp"

#include <csignal>
#include <filesystem>
#include <fstream>
#include <memory>

#include <dep/json.hpp>
#include <zmq.hpp>

#include <flame/log.hpp>
#include <flame/config.hpp>
#include <flame/config_def.hpp>
#include "manager.hpp"


using namespace std;
using json = nlohmann::json;

static flame::config_loader* _config_loader = nullptr;


/**
 * @brief cleanup and termination
 * 
 */
void cleanup(){
    if(_config_loader)
        delete _config_loader;
    manager->uninstall();
}

void cleanup_and_exit(){
    cleanup();
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

    try{
        _config_loader = new flame::config_loader(config_path);
        if(install_bundle()){
            console::info("Successfully installed");
            run_bundle();
        }
    }
    catch (const std::exception& e){
        console::critical("{}", e.what());
        return false;
    }

    return true;

}

bool install_bundle(const char* bundle){

    // install by configuration file
    if(!bundle && _config_loader){
        fs::path _bundle_path = _config_loader->get_config_path().parent_path() / fs::path(_config_loader->get_bundle_name());
        console::info("Bundle Repository : {}", _bundle_path.string());
        
        if(fs::is_directory(_bundle_path)){
            console::info("Now installing '{}' bundle..", _config_loader->get_bundle_name());

            // install bundle
            manager->install(_bundle_path);

        }
        else{
            console::critical("{} bundle cannot be found. Check your configurations.", _config_loader->get_bundle_name());
            return false;
        }
    }
    else{
        console::warn("Manual installation does not support yet.");
        return false;
    }

    return true;
}

void run_bundle(){
    manager->start_bundle_service();
}