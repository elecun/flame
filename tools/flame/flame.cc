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

    cxxopts::Options options("Flame bundle executor options");

    /* program options */
    options.add_options()
        ("c,config", "user configuration file(*.conf)", cxxopts::value<string>()->default_value("default.conf"))
        ("l,logfile", "save logs in file(flame.log)")
        ("v,verbose", "verbose log level [trace|debug|info|warn|err|critical|off]", cxxopts::value<string>()->default_value("trace"))
        ("h,help", "Print usage");

    auto optval = options.parse(argc, argv);
    if(optval.count("help")){
        cout << options.help() << endl;
        exit(EXIT_SUCCESS);
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

    /* verbose control arguments */
    auto console_sink = std::make_shared<logger::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
    std::vector<logger::sink_ptr> sinks { console_sink };

    string _verbose_level = optval["verbose"].as<string>();
    int _verbose_level_i = str2level(_verbose_level);
    
    /* logfile configuration */
    if(optval.count("logfile")) {
        auto file_sink = std::make_shared<logger::sinks::basic_file_sink_mt>("flame.log", true);
        file_sink->set_level(static_cast<logger::level::level_enum>(_verbose_level_i));
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
        sinks.push_back(file_sink);
    }

    /* set logger set */
    auto logger = std::make_shared<logger::logger>("flame", sinks.begin(), sinks.end());
    logger::set_default_logger(logger);
    logger::set_level(static_cast<logger::level::level_enum>(_verbose_level_i));

    /* program begin */
    logger::info("FLAME Execution Engine {} (built {}/{})",_FLAME_VER_, __DATE__, __TIME__);
    logger::info("Verbose Level : {}({})", _verbose_level, _verbose_level_i);

    try{
        string _config_file = optval["config"].as<string>();
        if(!_config_file.empty()){
            if(init(_config_file.c_str())){
                logger::info("Bundle is now working...");
                pause(); //wait until getting SIGINT
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