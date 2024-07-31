/**
 * @file driver.cc
 * @author Byunghun Hwang
 * @brief Component Driver
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "driver.hpp"
#include <dlfcn.h>
#include <signal.h>
#include <chrono>
#include <flame/core/profile.hpp>
#include <flame/util/file.hpp>
#include <stdexcept>
#include <flame/core/registry.hpp>
#include <flame/core/def.hpp>
#include <flame/log.hpp>
#include <filesystem> //to use c++17 filesystem

namespace fs = std::filesystem;
using namespace std;
using namespace flame::core;


#if defined(linux) || defined(__linux) || defined(__linux__)
    static const int SIG_RUNTIME_TRIGGER = (SIGRTMIN);     //signal #64 : Runtime Error
    static const int SIG_PAUSE_TRIGGER = (SIGRTMIN-1);     //signal #63 : Pause Process
    static const int SIG_RESUME_TRIGGER = (SIGRTMIN-2);    //signal #62 : Resume Process
    static const int SIG_STOP_TRIGGER = (SIGRTMAX-3);      //signal #61 : Process Termination
#endif


namespace flame::core::task {

    /**
     * @brief Construct a new driver instance with its profile
     * 
     * @param taskname Task Name, It must be unique.
     */
    driver::driver(const char* taskname){
        try {
            if(load(taskname)){
                if(_taskImpl){
                    /* load profile */
                    if(registry->find(_PATH_BIN_DIR_)){
                        fs::path _bin = registry->get<string>(_PATH_BIN_DIR_);
                        fs::path _task = _bin / fs::path{fmt::format("{}{}",taskname, _FILE_EXT_COMPONENT_)};
                        fs::path _profile = _bin / fs::path{fmt::format("{}{}",taskname, _FILE_EXT_PROFILE_)};

                        if(fs::exists(_profile)){
                            _taskImpl->_profile = make_unique<core::profile>(_profile.c_str());
                            _taskImpl->_taskname = taskname;
                            _taskImpl->set_status(task::status_d::IDLE);
                        }
                        else {
                            logger::error("<{}> profile does not exist.", taskname);
                        }
                    }
                }
                else {
                    logger::error("Component successfully loaded, but the instance has null pointer.");
                }
            }
            else {
                _taskImpl = nullptr;
                logger::error("Component load failed.");
            }
        }
        catch(std::runtime_error& e){
            logger::error("<{}> driver cannot be loadded ({})", taskname, e.what());
        }
    }

    driver::driver(task::runnable* instance){
        try {
            _taskImpl = std::move(instance);
        }
        catch(std::runtime_error& e){
            if(instance){
                logger::error("<{}> driver cannot be loadded ({})", instance->get_name(), e.what());
            }
            
        }
    }

    driver::driver(fs::path component){

    }

    driver::~driver(){
        unload();
    }


    bool driver::configure(){
        try {
            if(_taskImpl){
                if(_taskImpl->rtype==task::rtype_d::RT){
                    _taskImpl->_option.check_jitter = _taskImpl->get_profile()->data["info"]["policy"]["check_jitter"].get<bool>();
                    _taskImpl->_option.check_overrun = _taskImpl->get_profile()->data["info"]["policy"]["check_overrun"].get<bool>();
                }
                
                return _taskImpl->configure();
            }
        }
        catch(const std::runtime_error& e){
            logger::error("Runtime Error(configure) : {}", e.what());
        }

        return false;
    }

    void driver::execute(){
        if(_taskImpl) {
            if(_taskImpl->get_status()==task::status_d::STOPPED || _taskImpl->get_status()==task::status_d::IDLE){
                if(_taskImpl->rtype==task::rtype_d::NT || _taskImpl->rtype==task::rtype_d::RT){
                    if(_taskImpl->_profile){
                        unsigned long long rtime = _taskImpl->_profile->data["info"]["cycle_ns"].get<unsigned long long>();
                        logger::info("<{}> Time Period : {} ns",_taskImpl->get_name(), rtime);
                        set_rt_timer(rtime);
                        _ptrThread = new thread{ &flame::core::task::driver::do_process, this };
                    }
                }
                else {
                    logger::info("<{}> task is not a runnable(periodic) task.", _taskImpl->get_name());
                }
            }
            else {
                logger::warn("<{}> is still working. Task should be on STOPPED or IDLE state for execution.", _taskImpl->get_name());
            }
        }
        else {
            logger::error("Invalid task instance. The instance is null.");
        }
    }

    void driver::cleanup(){
        timer_delete(_timer_id);    //delete timer
        logger::info("Cleanup <{}>", _taskImpl->get_name());
        _taskImpl->set_status(task::status_d::STOPPED);
        if(_taskImpl)
            _taskImpl->cleanup();
        unload();
    }

    void driver::pause(){
        if(_taskImpl){
            logger::warn("Not support yet.");
        }
    }

    void driver::resume(){
        if(_taskImpl){
            logger::warn("Not support yet.");
        }
    }

    bool driver::load(const char* taskname){

        if(!taskname){
            logger::warn("Task was not specified. It must be requred.", taskname);
            return false;
        }

        if(registry->find(_PATH_BIN_DIR_)){

            fs::path _bin = registry->get<string>(_PATH_BIN_DIR_);
            fs::path _task = _bin / fs::path{fmt::format("{}{}",taskname, _FILE_EXT_COMPONENT_)};
            fs::path _profile = _bin / fs::path{fmt::format("{}{}",taskname, _FILE_EXT_PROFILE_)};

            // 1. check file existance
            if(!fs::exists(_task)){
                logger::error("{}{} doest not exist. please check path or task file.", taskname, _FILE_EXT_COMPONENT_);
                return false;
            }

            if(!fs::exists(_profile)){
                logger::error("{}{} doest not exist. please check path or profile file.", taskname, _FILE_EXT_PROFILE_);
                return false;
            }
            
            // 2. open component(shared library)
            _task_handle = dlopen(_task.c_str(), RTLD_LAZY|RTLD_LOCAL);
            if(_task_handle){
                create_task pfcreate = (create_task)dlsym(_task_handle, "create");
                if(!pfcreate){
                    logger::error("{} instance cannot be created", taskname);
                    dlclose(_task_handle);
                    _task_handle = nullptr;
                    return false;
                }
                _taskImpl = pfcreate();
                return true;
            }
            else {
                logger::error("{} load error occured : {}", taskname, dlerror());
                dlclose(_task_handle);
                _task_handle = nullptr;
            }
        }
        else {
            logger::error("Cannot find PATH in config file. You should define BIN_DIR.");
        }

        return false;
    }

    //unload task component
    void driver::unload(){
        if(_taskImpl){
            release_task pfrelease = (release_task)dlsym(_task_handle, "release");
            if(pfrelease)
                pfrelease();
            
            _taskImpl = nullptr;
        }
        if(_task_handle){
            dlclose(_task_handle);
            _task_handle = nullptr;
        }
    }

    //all component has own rt timer
    void driver::set_rt_timer(unsigned long long nsec){
        
        /* Set and enable alarm */ 
        _sig_evt.sigev_notify = SIGEV_SIGNAL; 
        _sig_evt.sigev_signo = SIG_RUNTIME_TRIGGER; 
        _sig_evt.sigev_value.sival_ptr = _timer_id; 
        if(timer_create(CLOCK_REALTIME, &_sig_evt, &_timer_id)==-1)
            logger::error("timer create error");
    
        const unsigned long long nano = (1000000000L);
        _time_spec.it_value.tv_sec = nsec / nano;
        _time_spec.it_value.tv_nsec = nsec % nano;
        _time_spec.it_interval.tv_sec = nsec / nano;
        _time_spec.it_interval.tv_nsec = nsec % nano;

        if(timer_settime(_timer_id, 0, &_time_spec, nullptr)==-1)
            logger::error("timer setting error");
    }

    //concreate process impl.
    void driver::do_process() {

        //signal set for threading
        sigset_t thread_sigmask;
        sigemptyset(&thread_sigmask);
        sigaddset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
        sigaddset(&thread_sigmask, SIG_PAUSE_TRIGGER);
        sigaddset(&thread_sigmask, SIG_RESUME_TRIGGER);
        sigaddset(&thread_sigmask, SIG_STOP_TRIGGER);
        int _sig_no;

        while(1){
            sigwait(&thread_sigmask, &_sig_no);
            if(_sig_no==SIG_RUNTIME_TRIGGER){
                _taskImpl->set_status(task::status_d::WORKING);
                //auto t_now = std::chrono::high_resolution_clock::now();
                if(_taskImpl){
                    _taskImpl->execute();
                }
                //kauto t_elapsed = std::chrono::high_resolution_clock::now();
                //spdlog::info("Processing Time : {} sec", std::chrono::duration<double, std::chrono::seconds::period>(t_elapsed - t_now).count());
            }
            else if(_sig_no==SIG_PAUSE_TRIGGER) {
                sigdelset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
                _taskImpl->set_status(task::status_d::PAUSED);
                if(_taskImpl){
                    _taskImpl->pause();
                }

            }
            else if(_sig_no==SIG_RESUME_TRIGGER) {
                _taskImpl->set_status(task::status_d::WORKING);
                if(_taskImpl){
                    _taskImpl->resume();
                }
                sigaddset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
            }

            _taskImpl->set_status(task::status_d::IDLE);    
        }
    }
} /* namespace */