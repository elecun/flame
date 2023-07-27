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


#include <iostream>
#include <string>
#include <csignal>
#include <vector>
#include <flame/log.hpp>
#include <flame/core.hpp>
#include <3rdparty/cxxopts.hpp>
#include <sys/mman.h>

#include "instance.hpp"

using namespace std;

void terminate(){
    app::cleanup();
    exit(EXIT_SUCCESS);
}

/* signal callback functions */
void signal_callback(int sig) {
    switch(sig){
        case SIGSEGV: { console::warn("Signal : Segmentation violation"); } break;
        case SIGABRT: { console::warn("Signal : Abnormal termination"); } break;
        case SIGKILL: { console::warn("Signal : Process killed"); } break;
        case SIGBUS: { console::warn("Signal : Bus Error"); } break;
        case SIGTERM: { console::warn("Signal : Termination requested"); } break;
        case SIGINT: { console::warn("Signal : Interrupted"); } break;
        default:
        console::info("Cleaning up the program");
    }
    ::terminate(); 
}

int main(int argc, char* argv[])
{
    string desc = fmt::format("Version {} (built {}/{})", _FLAME_VER_, __DATE__, __TIME__);
    cxxopts::Options options("Execution Engine with Flame Dev. Framework", desc.c_str());
    options.add_options()
        ("c,config", "Application start with configuration file(*.conf)", cxxopts::value<string>())
        ("h,help", "Print usage");

    auto optval = options.parse(argc, argv);
    if(optval.count("help")){
        std::cout << options.help() << std::endl;
        exit(EXIT_SUCCESS);
    }

    console::stdout_color_st("console"); // for log printing on console

    // signal connect to callback
    const int signals[] = { SIGINT, SIGTERM, SIGBUS, SIGKILL, SIGABRT, SIGSEGV };
    for(const int& s:signals)
        signal(s, signal_callback);

    sigset_t sigmask;
    if(!sigfillset(&sigmask)){
        for(int signal:signals)
        sigdelset(&sigmask, signal); //delete signal from mask
    }
    else {
        console::error("Signal Handling Error");
        ::terminate(); //if failed, do termination
    }

    if(pthread_sigmask(SIG_SETMASK, &sigmask, nullptr)!=0){ // signal masking for main thread
        console::error("Signal Masking Error");
        ::terminate();
    }

    mlockall(MCL_CURRENT|MCL_FUTURE); //avoid memory swaping
    
    /* option arguments */
    string _config {""};
    vector<string> _comps;

    if(optval.count("config")){
        _config = optval["config"].as<string>();
    }

    try{
        if(!_config.empty()){
            if(flame::app::initialize(_config.c_str())){
                flame::app::run();
                pause(); //wait until getting SIGINT
            }
        }
    }
    catch(const std::exception& e){
        console::error("Exception : {}", e.what());
    }

    ::terminate();
    return EXIT_SUCCESS;
}