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
#include <iostream>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <csignal>

#include <dep/cxxopts.hpp>
#include <flame/log.hpp>
#include <flame/version.hpp>

#include "instance.hpp"
#include <flame/config.hpp>

using namespace std;


int main(int argc, char* argv[])
{
    console::stdout_color_st("console");
    console::info("FLAME Execution Engine {} (built {}/{})",_FLAME_VER_, __DATE__, __TIME__);

    cxxopts::Options options("Flame options");

    options.add_options()
        ("c,config", "User Configuration File(*.conf)", cxxopts::value<string>())
        ("h,help", "Print usage");

    auto optval = options.parse(argc, argv);
    if(optval.count("help")){
        cout << options.help() << endl;
        exit(EXIT_SUCCESS);
    }

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
        cleanup_and_exit();
    }

    if(pthread_sigmask(SIG_SETMASK, &sigmask, nullptr)!=0){ // signal masking for main thread
        cleanup_and_exit();
    }

    mlockall(MCL_CURRENT|MCL_FUTURE); //avoid memory swaping

    /* option arguments */
    string _config {""};

    if(optval.count("config")){ _config = optval["config"].as<string>(); }

    try{
        if(!_config.empty()){
            if(init(_config.c_str())){
                console::info("Bundle is now working...");
                pause(); //wait until getting SIGINT
            }
        }
        else{
            console::warn("No Arguments. Burner will run with default configuration");
        }
        
    }
    catch(const std::exception& e){
        console::error("Exception : {}", e.what());
    }

    cleanup_and_exit();
    return EXIT_SUCCESS;
}