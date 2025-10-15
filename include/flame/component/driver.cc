
#include "driver.hpp"

#include <csignal>
#include <dlfcn.h>
#include <chrono>
#include <stdexcept>
#include <chrono>
#include <string>

#include <flame/config_def.hpp>
#include <flame/log.hpp>

using namespace std;

#if defined(linux) || defined(__linux) || defined(__linux__)
    static int SIG_RUNTIME_TRIGGER = (SIGRTMIN);
#endif

// [important] bundle should be handle the inproc context
pipe_context flame::component::driver::inproc_pipeline_context = pipe_context(10);

namespace flame::component {

    driver::driver(path component_path){
        try{
            fs::path cobject = component_path.replace_extension(__COMPONENT_FILE_EXT__);
            fs::path cprofile = component_path.replace_extension(__PROFILE_FILE_EXT__);

            if(load(cobject)){
                assert(_componentImpl!=nullptr);
                
                /* set important parameters and profile first */
                _componentImpl->_profile = make_unique<component::profile>(cprofile);
                _componentImpl->_name = component_path.stem().string();
                _componentImpl->_status = dtype_status::STOPPED;

                /* get dataport descriptions in profile */
                json _dataport = _componentImpl->get_profile()->dataport();

                /* assign context with number of io threads */
                int n_socket_io = static_cast<int>(_dataport.size());
                _componentImpl->pipeline_context = make_unique<pipe_context>(n_socket_io);
                _componentImpl->inproc_pipeline_context = &this->inproc_pipeline_context;
                logger::info("Component <{}> has pipeline context with {} I/O Threads", component_path.filename().string(), n_socket_io);

                /* assign user defined data port */
                if(!_dataport.empty()){
                    for(auto& [portname, parameter] : _dataport.items()){
                        string socket_type = parameter["socket_type"].get<string>();
                        string transport = parameter["transport"].get<string>();
                        int q_size = parameter.value("queue_size", 1000);
                        int tout = parameter.value("timeout_ms", 500);
                        
                        /* create port(socket) for inproc transport */
                        if(!transport.compare("inproc")){
                            _componentImpl->create_port_inproc(portname, str2type(socket_type), q_size, tout, portname);
                        }

                        /* create port(socket) for ipc, but not support yet */
                        else if(!transport.compare("ipc")){
                            logger::warn("ipc transport is not supported yet.");
                        }

                        /* create port(socket) for tcp */
                        else if(!transport.compare("tcp")){
                            int port = parameter.value("port", 5555);
                            string host = parameter.value("host", "*");
                            _componentImpl->create_port_tcp( 
                                                portname, /* socket name*/ 
                                                str2type(socket_type), /* socket type */ 
                                                q_size, /* buffer size */ 
                                                host, /* host address to bind */
                                                port, /* host port to bind */
                                                tout, /* rcv timeout */
                                                portname /* topic if subscriber */);
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
        try{
            /* clear all */
            _componentImpl->close_port();

            /* pipeline termination */
            _componentImpl->pipeline_context->close();
        }
        catch(const zmq::error_t& e){
            logger::error("Pipeline error : {}", e.what());
        }
        catch(const std::runtime_error& e){
            logger::error("Runtime Error : {}", e.what());
        }

        /* unload component */
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
                
                /* shutdown pipeline context, all socket will be closed */
                //_componentImpl->pipeline_context->shutdown();

                /* call after pipeline context shutdown */
                return _componentImpl->on_close();
            }
        }
        catch(const std::runtime_error& e){
            logger::error("Runtime Error(on_close) : {}", e.what());
        }
        catch(const zmq::error_t& e){
            logger::error("Pipeline error : {}", e.what());
        }

    }

    void driver::on_message(const message_t& msg){
        try {
            if(_componentImpl){
                // The driver should be responsible for receiving messages and passing them.
                // For now, we pass an empty message to satisfy the interface.
                flame::component::message_t msg;
                return _componentImpl->on_message(msg);
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
                logger::error("<{}> component cannot be found.", component_path.filename().string());
                return false;
            }

            _component_handle = dlopen(component_path.string().c_str(), RTLD_LAZY|RTLD_LOCAL);
            if(!_component_handle){
                logger::error("<{}> {}", component_path.filename().string(), dlerror());
                return false;
            }

            create_component pfcreate = (create_component)dlsym(_component_handle, "create");
            const char* dlsym_error = dlerror();
            if(dlsym_error){
                logger::error("<{}> component instance cannot be created", component_path.filename().string());
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
        logger::info("[{}] Assigned Trigger Signal ID : {}", get_name(), _signal_id);
    
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
                // auto t_now = std::chrono::high_resolution_clock::now();
                if(_componentImpl){
                    _componentImpl->on_loop();
                }
                // auto t_elapsed = std::chrono::high_resolution_clock::now();
                //logger::info("Processing Elapsed Time : {} sec", std::chrono::duration<double, std::chrono::milliseconds::period>(t_elapsed - t_now).count());
            }
        }

    }

} /* namespace */