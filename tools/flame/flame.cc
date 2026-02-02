/**
 * @file flame.cc
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Flame Execution Engine
 * @version 0.1
 * @date 2023-07-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <csignal>
#include <dep/cxxopts.hpp>
#include <flame/log.hpp>
#include <flame/version.hpp>
#include <flame/def.hpp>

#include "instance.hpp"
#include "provider.hpp"
#include <flame/config.hpp>
#include <flame/common/zpipe.hpp>
#include <dep/json.hpp>

using namespace std;
using json = nlohmann::json;




int main(int argc, char* argv[])
{

    cxxopts::Options options("Flame bundle executor options");

    /* program options */
    options.add_options()
        ("c,config", "user configuration file(*.conf)", cxxopts::value<string>()->default_value("default.conf"))
        ("v,verbose", "verbose log level [trace|debug|info|warn|err|critical|off]", cxxopts::value<string>()->default_value("trace"))
        ("show", "show information", cxxopts::value<string>()->implicit_value("true"))
        ("h,help", "Print usage");

    auto optval = options.parse(argc, argv);
    if(optval.count("help")){
        console::info("{}", options.help());
        exit(EXIT_SUCCESS);
    }
    
    // Commands Handler
    if(optval.count("show")){
        // check sub-commands
        // argc > 2
        // argv[2] == "bundle" ?
        bool show_bundle = false;
        for(int i=0;i<argc;i++){
            if(string(argv[i])=="bundle") show_bundle = true;
        }


        if(show_bundle){
            string _config_file = optval["config"].as<string>();
            config.load(_config_file.c_str());

            flame::StateProvider provider;
            provider.start_subscribe();
            
            console::info("Listening for flame status... (Press Ctrl+C to exit)");
            while(true){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    /* set signals to catch the abnormal interrupts */
    const int signals[] = { SIGINT, SIGTERM, SIGBUS, SIGKILL, SIGABRT, SIGSEGV };
    for(const int& s:signals)
        signal(s, signal_callback);

    sigset_t sigmask;
    if(!sigfillset(&sigmask)){
        for(int signal:signals)
            sigdelset(&sigmask, signal); //delete signal from mask
    }
    else {
        cleanup_and_exit();
    }

    if(pthread_sigmask(SIG_SETMASK, &sigmask, nullptr)!=0){ // signal masking for main thread
        cleanup_and_exit();
    }

    mlockall(MCL_CURRENT|MCL_FUTURE); //avoid memory swaping

    /* logger configuration */
    string _verbose_level = optval["verbose"].as<string>();
    int _verbose_level_i = str2level(_verbose_level);
    create(_verbose_level);

    /* program begin */
    logger::info("FLAME Execution Engine {} (built {}/{})",_FLAME_VER_, __DATE__, __TIME__);
    logger::info("Verbose Level : {}({})", _verbose_level, _verbose_level_i);

    try{
        string _config_file = optval["config"].as<string>();
        if(!_config_file.empty()){
            if(init(_config_file.c_str())){
                while(!g_shutdown_requested.load()) {
                    pause(); 
                }
            }
        }
        else{
            logger::warn("No Arguments. Burner will run with default configuration");
        }
        
    }
    catch(const std::exception& e){
        logger::error("Exception : {}", e.what());
    }

    cleanup_and_exit();
    return EXIT_SUCCESS;
}