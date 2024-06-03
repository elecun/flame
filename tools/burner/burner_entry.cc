/**
 * @file burner_entry.cc
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Burner entry
 * @version 0.1
 * @date 2024-05-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <csignal>

#include <dep/cxxopts.hpp>
#include <flame/interface.hpp>

#include "instance.hpp"

using namespace std;


/* Entry */
int main(int argc, char* argv[]){
    console::stdout_color_st("console");

    string desc = fmt::format("Burner with Flame RT (built {}/{})", __DATE__, __TIME__);
    console::info("{}",desc);

    cxxopts::Options options("Burner options", desc.c_str());

    options.add_options()
        ("c,config", "Global Configuration File(*.conf)", cxxopts::value<string>())
        ("h,help", "Print usage");

    auto optval = options.parse(argc, argv);
    if(optval.count("help")){
        cout << options.help() << endl;
        exit(EXIT_SUCCESS);
    }

    // signal connect to callback
    const int signals[] = { SIGINT, SIGTERM, SIGBUS, SIGKILL, SIGABRT, SIGSEGV };
    for(const int& s:signals)
        signal(s, flame_rt::signal_callback);

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
            if(flame_rt::init(_config.c_str())){
                flame_rt::run();
            }
        }
        else{
            console::warn("No Arguments. Burner will run with default configuration");
        }
        pause(); //wait until getting SIGINT
    }
    catch(const std::exception& e){
        console::error("Exception : {}", e.what());
    }

    ::terminate();
    return EXIT_SUCCESS;
}