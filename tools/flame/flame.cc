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
        ("show_status", "show information of the running flame instance")
        ("h,help", "Print usage");

    auto optval = options.parse(argc, argv);
    if(optval.count("help")){
        console::info("{}", options.help());
        exit(EXIT_SUCCESS);
    }
    
    // Commands Handler
    if(optval.count("show_status")){
        
        auto pipe = flame::pipe::create_pipe(1);
        auto sock = make_shared<flame::pipe::AsyncZSocket>("cli_status", flame::pipe::Pattern::DEALER);

        bool response_received = false;
        if(sock->create(pipe)){
            // callback lambda
            sock->set_message_callback([&](const vector<string>& msg){
                if(msg.empty()) return;

                string rep_str;
                if(msg.size() > 1 && msg[0].empty()){
                    rep_str = msg[1];
                } else if(msg.size() == 1) {
                    rep_str = msg[0];
                } 
                else {
                    return; // invalid format?
                }

                try {
                    auto j = json::parse(rep_str);
                    int count = j["count"];
                    auto components = j["components"];

                    console::info("----------------------------------------------------------------------");
                    console::info(" Running Flame Instance Info");
                    console::info("----------------------------------------------------------------------");
                    console::info("{:<20} : {}", " PID File", "/tmp/flame.pid"); 
                    console::info("{:<20} : {}", " Status", j["status"].get<string>());
                    console::info("{:<20} : {}", " Component Count", count);
                    console::info("----------------------------------------------------------------------");
                    console::info("{:<30}{:<20}{:<20}", " Name", " Type", " Status");
                    console::info("----------------------------------------------------------------------");
                    for(const auto& c : components){
                        console::info("{:<30}{:<20}{:<20}", 
                            c["name"].get<string>(), 
                            c["type"].get<string>(), 
                            c["status"].get<string>());
                    }
                    console::info("----------------------------------------------------------------------");
                    response_received = true;
                }
                catch(const json::exception& e){
                    console::warn("Invalid response from flame instance. (JSON Parse Error: {})", e.what());
                    console::warn("Raw Response: {}", rep_str);
                    response_received = true; // exit anyway
                }
                catch(...){
                    console::warn("Invalid response from flame instance. (Unknown Error)");
                    console::warn("Raw Response: {}", rep_str);
                    response_received = true; // exit anyway
                }
            });

            // Connect
            // FLAME_MONITOR_IPC_ADDR is likely "ipc:///tmp/..."
            // zpipe's join takes transport, address, port.
            // Parse FLAME_MONITOR_IPC_ADDR
            string ipc_addr = flame::def::FLAME_MONITOR_IPC_ADDR;
            string transport = "ipc";
            string address = ipc_addr.substr(ipc_addr.find("://") + 3);
            int port = 0; // not used for IPC

            if(sock->join(flame::pipe::Transport::IPC, address, 0)){
                // Send Request
                vector<string> req;
                req.push_back(""); // Emulate REQ envelope
                req.push_back("STATUS");
                
                sock->dispatch(req);

                // Wait for response with timeout
                int timeout_ms = 1000;
                int elapsed = 0;
                while(!response_received && elapsed < timeout_ms){
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    elapsed += 10;
                }
                
                if(!response_received){
                    console::warn("No flame process running (Timeout).");
                }

            } else {
                console::warn("Failed to connect to flame instance.");
            }

            sock->close();
        }
        
        flame::pipe::destroy_pipe();
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
                console::info("Bundle is now working...");
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