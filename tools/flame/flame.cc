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
#include <iomanip>

#include <dep/cxxopts.hpp>
#include <flame/log.hpp>
#include <flame/version.hpp>
#include <flame/def.hpp>

#include "instance.hpp"
#include <flame/config.hpp>

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
        ("l,logfile", "save logs in file(flame.log)")
        ("v,verbose", "verbose log level [trace|debug|info|warn|err|critical|off]", cxxopts::value<string>()->default_value("trace"))
        ("show_status", "show information of the running flame instance")
        ("h,help", "Print usage");

    auto optval = options.parse(argc, argv);
    if(optval.count("help")){
        cout << options.help() << endl;
        exit(EXIT_SUCCESS);
    }
    
    // Commands Handler
    // Commands Handler
    if(optval.count("show_status")){
        
        auto pipe = flame::pipe::create_pipe(1);
        auto sock = make_shared<flame::pipe::AsyncZSocket>("cli_status", flame::pipe::Pattern::DEALER);

        bool response_received = false;
        if(sock->create(pipe)){
            // callback lambda
            sock->set_message_callback([&](const vector<string>& msg){
                if(msg.empty()) return;

                // DEALER receives what REP sent. REP usually sends [result] (or [empty][result] if REQ envelope)
                // However, our monitor is REP.
                // REP -> DEALER: REP will strip the envelope from REQ.
                // DEALER should send [empty][content] to mimic REQ if it wants to be treated as REQ by REP?
                // Actually if monitor uses REP, it expects an identity frame if it's async? No, ZMQ_REP handles envelopes automatically.
                // If we use DEALER to talk to REP:
                // We must send [empty frame][request].
                // REP receives [request].
                // REP sends [reply].
                // DEALER receives [empty frame][reply] because REP prepends the envelope? 
                // Wait, REQ adds empty delimiter. DEALER does not.
                // So DEALER must send ["", "STATUS"].
                
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

                    cout << "----------------------------------------------------------------------" << endl;
                    cout << " Running Flame Instance Info" << endl;
                    cout << "----------------------------------------------------------------------" << endl;
                    cout << left << setw(20) << " PID File" << " : " << "/tmp/flame.pid" << endl; // Assuming standard PID location or dummy
                    cout << left << setw(20) << " Status" << " : " << j["status"].get<string>() << endl;
                    cout << left << setw(20) << " Component Count" << " : " << count << endl;
                    cout << "----------------------------------------------------------------------" << endl;
                    cout << left << setw(30) << " Name" << setw(20) << " Type" << setw(20) << " Status" << endl;
                    cout << "----------------------------------------------------------------------" << endl;
                    for(const auto& c : components){
                        cout << left << setw(30) << c["name"].get<string>() 
                             << setw(20) << c["type"].get<string>() 
                             << setw(20) << c["status"].get<string>() << endl;
                    }
                    cout << "----------------------------------------------------------------------" << endl;
                    response_received = true;
                }
                catch(const json::exception& e){
                    cout << "Invalid response from flame instance. (JSON Parse Error: " << e.what() << ")" << endl;
                    cout << "Raw Response: " << rep_str << endl;
                    response_received = true; // exit anyway
                }
                catch(...){
                    cout << "Invalid response from flame instance. (Unknown Error)" << endl;
                    cout << "Raw Response: " << rep_str << endl;
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

            if(sock->join("ipc", address, 0)){
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
                    cout << "No flame process running (Timeout)." << endl;
                }

            } else {
                cout << "Failed to connect to flame instance." << endl;
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