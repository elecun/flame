
#include "driver.hpp"

#include <csignal>
#include <dlfcn.h>
#include <chrono>
#include <stdexcept>
#include <chrono>
#include <string>

#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace std;

#if defined(linux) || defined(__linux) || defined(__linux__)
    static int SIG_RUNTIME_TRIGGER = (SIGRTMIN);
#endif

namespace flame::component {

    driver::driver(path component_path, flame::pipe_context* pipeline){
        try{
            fs::path cobject = component_path.replace_extension(__COMPONENT_FILE_EXT__);
            fs::path cprofile = component_path.replace_extension(__PROFILE_FILE_EXT__);

            if(load(cobject)){
                assert(_componentImpl!=nullptr);
                //1. set parameters
                _componentImpl->_profile = make_unique<component::profile>(cprofile);
                _componentImpl->_name = component_path.stem().string();
                _componentImpl->_status = dtype_status::STOPPED;

                // 2. assign inproc dataport
                json _dataport = _componentImpl->get_profile()->dataport();
                if(!_dataport.empty()){
                    for(auto& [name, value] : _dataport.items()){
                        string socket_type = value["socket_type"].get<string>();
                        string transport = value["transport"].get<string>();
                        int q_size = value.value("queue_size", 1000);
                        
                        // 2.1 build data port [inproc]
                        if(!transport.compare("inproc")){
                            _componentImpl->create_port(pipeline, name, str2type(socket_type), q_size, name);
                        }

                        // 2.2 tcp data port
                        else if(!transport.compare("tcp")){
                            int port = value.value("port", 5555);
                            string host = value.value("host", "*");
                            pipe_context* pipe = _componentImpl->create_pipe(name.c_str());
                            _componentImpl->create_port(pipe, 
                                                name, /* socket name*/ 
                                                socket_type, /* socket type */ 
                                                q_size, /* buffer size */ 
                                                host, /* host address to bind */
                                                port, /* host port to bind */
                                                name /* topic if subscriber */);
                        }
                    }
                }
                
            }
        }
        catch (std::runtime_error& e){
            logger::error("<{}> component driver cannot be coupled", component_path.filename().string());
            throw std::runtime_error(e.what());
        }
    }

    driver::~driver(){

        _componentImpl->destory_pipe();

        unload();
    }

    bool driver::on_init(){
        try {
            if(_componentImpl){
                return _componentImpl->on_init();
            }
        }
        catch(const std::runtime_error& e){
            logger::error("Runtime Error(on_init) : {}", e.what());
        }

        return false;
    }

    void driver::on_loop(){

        try {
            if(_componentImpl){
                if(_componentImpl->get_status()==flame::component::dtype_status::STOPPED){
                    unsigned long long _rtime = _componentImpl->get_profile()->raw()[__PROFILE_RT_CYCLE_NS__].get<unsigned long long>();
                    set_rt_timer(_rtime);
                    _ptrThread = new thread{ &flame::component::driver::do_cycle, this };
                }
            }
        }
        catch(const std::runtime_error& e){
            logger::error("Runtime Error(on_loop) : {}", e.what());
        }
    }

    void driver::on_close(){
        try {
            timer_delete(_timer_id);    //delete timer
            if(_componentImpl){
                return _componentImpl->on_close();
            }
        }
        catch(const std::runtime_error& e){
            logger::error("Runtime Error(on_close) : {}", e.what());
        }

    }

    void driver::on_message(){
        try {
            if(_componentImpl){
                return _componentImpl->on_message();
            }
        }
        catch(const std::runtime_error& e){
            logger::error("Runtime Error(on_message) : {}", e.what());
        }
    }

    bool driver::load(fs::path component_path){ // component path = *.comp absolute path
        try{
            // not exist
            if(!fs::exists(component_path)){
                logger::error("{} component cannot be found.", component_path.filename().string());
                return false;
            }

            logger::info("{}", component_path.c_str());
            _component_handle = dlopen(component_path.string().c_str(), RTLD_LAZY|RTLD_LOCAL);
            if(!_component_handle){
                logger::error("<{}> {}", component_path.filename().string(), dlerror());
                return false;
            }

            create_component pfcreate = (create_component)dlsym(_component_handle, "create");
            const char* dlsym_error = dlerror();
            if(dlsym_error){
                logger::error("{} component instance cannot be created", component_path.filename().string());
                dlclose(_component_handle);
                _component_handle = nullptr;
                return false;
            }

            // create instance
            _componentImpl = pfcreate();
        }
        catch(std::runtime_error& e){
            logger::error("<{}> : {}", component_path.filename().string(), e.what());
        }

        return true;
    }

    //unload task component
    void driver::unload(){
        try{
            if(_componentImpl){
                release_component pfrelease = (release_component)dlsym(_component_handle, "release");

                // release instance
                if(pfrelease)
                    pfrelease();
                _componentImpl = nullptr;
            }

            if(_component_handle){
                dlclose(_component_handle);
                _component_handle = nullptr;
            }
        }
        catch(std::runtime_error& e){
            logger::error("component unload failed");
        }
    }

    void driver::set_rt_timer(unsigned long long nsec){
        
        /* Set and enable alarm */ 
        _signal_event.sigev_notify = SIGEV_SIGNAL; // notify for process, so all thread received the signal
        _signal_id = SIG_RUNTIME_TRIGGER;
        _signal_event.sigev_signo = SIG_RUNTIME_TRIGGER++; 
        _signal_event.sigev_value.sival_ptr = _timer_id; 
        if(timer_create(CLOCK_REALTIME, &_signal_event, &_timer_id)==-1)
            logger::error("timer create error");
        logger::info("[{}] Trigger Signal ID : {}", get_name(), _signal_id);
    
        const unsigned long long nano = (1000000000L);
        _time_spec.it_value.tv_sec = nsec / nano;
        _time_spec.it_value.tv_nsec = nsec % nano;
        _time_spec.it_interval.tv_sec = nsec / nano;
        _time_spec.it_interval.tv_nsec = nsec % nano;

        // start timer
        if(timer_settime(_timer_id, 0, &_time_spec, nullptr)==-1)
            logger::error("timer setting error");

    }

    void driver::do_cycle(){

        //signal set for threading
        sigset_t thread_sigmask;
        sigemptyset(&thread_sigmask);
        sigaddset(&thread_sigmask, _signal_id); //block SIG_RUNTIME_TRIGGER signal
        int _sig_no;

        while(1){
            sigwait(&thread_sigmask, &_sig_no); //wait until receive signal
            if(_sig_no==_signal_id){
                auto t_now = std::chrono::high_resolution_clock::now();
                if(_componentImpl){
                    _componentImpl->on_loop();
                }
                auto t_elapsed = std::chrono::high_resolution_clock::now();
                //logger::info("Processing Elapsed Time : {} sec", std::chrono::duration<double, std::chrono::milliseconds::period>(t_elapsed - t_now).count());
            }
        }

    }

} /* namespace */