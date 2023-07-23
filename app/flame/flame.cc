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
#include <flame/log.hpp>
#include <3rdparty/cxxopts.hpp>

#include "instance.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    string desc = fmt::format("Ver. {} (built {}/{})", _FLAME_VER_, __DATE__, __TIME__);
    cxxopts::Options options("OpenEdge Framework Engine", desc.c_str());
    options.add_options()
        ("c,config", "Application start with configuration file(*.conf)", cxxopts::value<string>())
        ("i,install", "Install Components", cxxopts::value<vector<string>>())
        ("u,uninstall", "Uninstall Components", cxxopts::value<vector<string>>())
        ("l,logfile", "Logging to file", cxxopts::value<string>())
        ("h,help", "Print usage");

    auto optval = options.parse(argc, argv);
    if(optval.count("help")){
        std::cout << options.help() << std::endl;
        exit(EXIT_SUCCESS);
    }

    console::stdout_color_st("console");

    const int signals[] = { SIGINT, SIGTERM, SIGBUS, SIGKILL, SIGABRT, SIGSEGV };
    for(const int& s:signals)
        signal(s, cleanup);

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
    string _logfile = {""};

    if(optval.count("config")){
        _config = optval["config"].as<string>();
    }
    else if(optval.count("install")){
        _comps = optval["install"].as<vector<string>>();
    }
    else if(optval.count("uninstall")){
        _comps = optval["uninstall"].as<vector<string>>();
    }
    else if(optval.count("logfile")){
        _logfile = optval["logfile"].as<string>();
        console::info("Logging to file : {}, but not support yet.", _logfile);
    }

    try{
        if(!_config.empty()){
            if(app::initialize(_config.c_str())){
                app::run();
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