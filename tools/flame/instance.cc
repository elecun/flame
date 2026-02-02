
#include "instance.hpp"

#include <csignal>
#include <execinfo.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <memory>

#include <dep/json.hpp>
#include <zmq.hpp>
#include <zmq_addon.hpp>

#include <flame/log.hpp>
#include <flame/config.hpp>
#include <flame/def.hpp>
#include "manager.hpp"
#include "provider.hpp"

using namespace std;
using json = nlohmann::json;

flame::StateProvider _provider;
volatile std::atomic<bool> g_shutdown_requested{false};

/**
 * @brief cleanup and termination
 * 
 */
void cleanup(){
    _provider.stop();
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
    if (sig == SIGSEGV || sig == SIGBUS || sig == SIGABRT || sig == SIGKILL) {
        const char* msg;
        switch(sig) {
            case SIGSEGV: msg = "\nError: Segmentation violation (SIGSEGV)\n"; break;
            case SIGBUS:  msg = "\nError: Bus Error (SIGBUS)\n"; break;
            case SIGABRT: msg = "\nError: Abnormal termination (SIGABRT)\n"; break;
            case SIGKILL: msg = "\nError: Process killed (SIGKILL)\n"; break;
            default:      msg = "\nError: Fatal Signal\n"; break;
        }
        // Async-signal-safe write
        if(write(STDERR_FILENO, msg, strlen(msg)) < 0) {} // ignore error

        // Print backtrace (depth : 20)
        void* array[20];
        size_t size;
        size = backtrace(array, 20);
        
        const char* bt_msg = "Stack trace:\n";
        if(write(STDERR_FILENO, bt_msg, strlen(bt_msg)) < 0) {} // ignore error
        
        backtrace_symbols_fd(array, size, STDERR_FILENO);
        
        _exit(EXIT_FAILURE);
    } 
    else if (sig == SIGINT || sig == SIGTERM) {
        g_shutdown_requested.store(true);
        const char* msg = (sig == SIGINT) ? "\nInterrupted (SIGINT)\n" : "\nTerminated (SIGTERM)\n";
        if(write(STDERR_FILENO, msg, strlen(msg)) < 0) {} // ignore error
    }
    else {
        g_shutdown_requested.store(true);
    }
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
    _provider.start();
    manager.start_bundle_service();
}