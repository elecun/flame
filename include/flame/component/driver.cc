
#include "driver.hpp"

#include <csignal>
#include <dlfcn.h>
#include <chrono>
#include <stdexcept>

#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace std;

#if defined(linux) || defined(__linux) || defined(__linux__)
    static const int SIG_RUNTIME_TRIGGER = (SIGRTMIN);
#endif

namespace flame::component {

    driver::driver(path component_path, zmq::context_t* dout_ctx){
        try{
            fs::path cobject = component_path.replace_extension(__COMPONENT_FILE_EXT__);
            fs::path cprofile = component_path.replace_extension(__PROFILE_FILE_EXT__);

            if(load(cobject)){
                assert(_componentImpl!=nullptr);
                _componentImpl->_profile = make_unique<component::profile>(cprofile);
                _componentImpl->_name = component_path.filename().string();
                _componentImpl->_status = dtype_status::STOPPED;

                // create data out port
                _dout_port_socket = new zmq::socket_t(*dout_ctx, ZMQ_PUSH);
                _dout_port_socket->bind(fmt::format("inproc://{}",_componentImpl->get_name()));
            }
        }
        catch (std::runtime_error& e){
            console::error("<{}> component driver cannot be coupled", component_path.filename().string());
            throw std::runtime_error(e.what());
        }
    }

    driver::~driver(){

        //clsoe data port
        _dout_port_socket->close();
        delete _dout_port_socket;

        unload();
    }

    bool driver::on_init(){
        try {
            if(_componentImpl){
                return _componentImpl->on_init();
            }
        }
        catch(const std::runtime_error& e){
            console::error("Runtime Error : {}", e.what());
        }

        return false;
    }

    void driver::on_loop(){
        // if(_taskImpl) {
        //     if(_taskImpl->get_status()==task::status_d::STOPPED || _taskImpl->get_status()==task::status_d::IDLE){
        //         if(_taskImpl->rtype==task::rtype_d::NT || _taskImpl->rtype==task::rtype_d::RT){
        //             if(_taskImpl->_profile){
        //                 unsigned long long rtime = _taskImpl->_profile->data["info"]["cycle_ns"].get<unsigned long long>();
        //                 console::info("<{}> Time Period : {} ns",_taskImpl->get_name(), rtime);
        //                 set_rt_timer(rtime);
        //                 _ptrThread = new thread{ &flame::core::task::driver::do_process, this };
        //             }
        //         }
        //         else {
        //             console::info("<{}> task is not a runnable(periodic) task.", _taskImpl->get_name());
        //         }
        //     }
        //     else {
        //         console::warn("<{}> is still working. Task should be on STOPPED or IDLE state for execution.", _taskImpl->get_name());
        //     }
        // }
        // else {
        //     console::error("Invalid task instance. The instance is null.");
        // }

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
            console::error("Runtime Error : {}", e.what());
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
            console::error("Runtime Error : {}", e.what());
        }

    }

    void driver::on_message(){
        try {
            if(_componentImpl){
                return _componentImpl->on_message();
            }
        }
        catch(const std::runtime_error& e){
            console::error("Runtime Error : {}", e.what());
        }
    }

    bool driver::load(fs::path component_path){ // component path = *.comp absolute path
        try{
            // not exist
            if(!fs::exists(component_path)){
                console::error("{} component cannot be found.", component_path.filename().string());
                return false;
            }

            _component_handle = dlopen(component_path.string().c_str(), RTLD_LAZY|RTLD_LOCAL);
            if(!_component_handle){
                console::error("{} component cannot be opened.", component_path.filename().string());
                dlerror();
                return false;
            }

            create_component pfcreate = (create_component)dlsym(_component_handle, "create");
            const char* dlsym_error = dlerror();
            if(dlsym_error){
                console::error("{} component instance cannot be created", component_path.filename().string());
                dlclose(_component_handle);
                _component_handle = nullptr;
                return false;
            }

            // create instance
            _componentImpl = pfcreate();
        }
        catch(std::runtime_error& e){
            console::error("<{}> : {}", component_path.filename().string(), e.what());
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
            console::error("component unload failed");
        }
    }

    void driver::set_rt_timer(unsigned long long nsec){
        
        /* Set and enable alarm */ 
        _signal_event.sigev_notify = SIGEV_SIGNAL; 
        _signal_event.sigev_signo = SIG_RUNTIME_TRIGGER; 
        _signal_event.sigev_value.sival_ptr = _timer_id; 
        if(timer_create(CLOCK_REALTIME, &_signal_event, &_timer_id)==-1)
            console::error("timer create error");
    
        const unsigned long long nano = (1000000000L);
        _time_spec.it_value.tv_sec = nsec / nano;
        _time_spec.it_value.tv_nsec = nsec % nano;
        _time_spec.it_interval.tv_sec = nsec / nano;
        _time_spec.it_interval.tv_nsec = nsec % nano;

        if(timer_settime(_timer_id, 0, &_time_spec, nullptr)==-1)
            console::error("timer setting error");
    }

    void driver::do_cycle(){

        //signal set for threading
        sigset_t thread_sigmask;
        sigemptyset(&thread_sigmask);
        sigaddset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
        int _sig_no;

        while(1){
            sigwait(&thread_sigmask, &_sig_no);
            if(_sig_no==SIG_RUNTIME_TRIGGER){
                //auto t_now = std::chrono::high_resolution_clock::now();
                if(_componentImpl){
                    _componentImpl->on_loop();
                }
                //kauto t_elapsed = std::chrono::high_resolution_clock::now();
                //spdlog::info("Processing Time : {} sec", std::chrono::duration<double, std::chrono::seconds::period>(t_elapsed - t_now).count());
            }
        }

    }

} /* namespace */