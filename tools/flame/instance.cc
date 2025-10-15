
#include "instance.hpp"

#include <csignal>
#include <filesystem>
#include <fstream>
#include <memory>

#include <dep/json.hpp>
#include <zmq.hpp>
#include <zmq_addon.hpp>

#include <flame/log.hpp>
#include <flame/config.hpp>
#include <flame/config_def.hpp>
#include "manager.hpp"


using namespace std;
using json = nlohmann::json;


/**
 * @brief cleanup and termination
 * 
 */
void cleanup(){
    manager.uninstall();
}

void cleanup_and_exit(){
    cleanup();
    logger::info("successfully terminated");
    logger::default_logger()->flush();
    
    exit(EXIT_SUCCESS);
}

/**
 * @brief signal event callback functions
 * 
 * @param sig signal number
 */
void signal_callback(int sig) {
    switch(sig){
        case SIGSEGV: { logger::warn("Segment violation"); } break;
        case SIGABRT: { logger::warn("Abnormal termination"); } break;
        case SIGKILL: { logger::warn("Process killed"); } break;
        case SIGBUS: { logger::warn("Bus Error"); } break;
        case SIGTERM: { logger::warn("Termination requested"); } break;
        case SIGINT: { logger::warn("Interrupted"); } break;
        default:
        logger::info("Cleaning up the program");
    }
    cleanup_and_exit();
    logger::info("test");
}

bool init(const char* config_path){

    try{
        config.load(config_path);
        
        if(install_bundle()){
            run_bundle();
        }
    }
    catch (const std::exception& e){
        logger::critical("[Engine] {}", e.what());
        return false;
    }

    return true;

}

bool install_bundle(const char* bundle){

    // install by configuration file
    if(!bundle && config.is_loaded()){
        fs::path _bundle_path = config.get_config_path().parent_path() / fs::path(config.get_bundle_name());
        logger::info("Bundle Repository : {}", _bundle_path.string());
        
        if(fs::is_directory(_bundle_path)){
            logger::info("Now installing '{}' bundle..", config.get_bundle_name());

            // install bundle
            manager.install(_bundle_path);

        }
        else{
            logger::critical("{} bundle cannot be found. Check your configurations.", config.get_bundle_name());
            return false;
        }
    }
    else{
        logger::warn("Manual installation does not support yet.");
        return false;
    }

    return true;
}

void run_bundle(){
    manager.start_bundle_service();
}