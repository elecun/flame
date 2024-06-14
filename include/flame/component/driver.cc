
#include "driver.hpp"

#include <csignal>
#include <dlfcn.h>
#include <chrono>
#include <stdexcept>

#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace std;

namespace flame::component {

    driver::driver(path component_path){
        try{
            fs::path cobject = component_path.replace_extension(__COMPONENT_FILE_EXT__);
            fs::path cprofile = component_path.replace_extension(__PROFILE_FILE_EXT__);

            if(load(cobject)){
                assert(_componentImpl!=nullptr);
                _componentImpl->_profile = make_unique<component::profile>(cprofile);
                _componentImpl->_name = component_path.filename().string();
                _componentImpl->_status = dtype_status::STOPPED;
            }
        }
        catch (std::runtime_error& e){
            console::error("<{}> component driver cannot be coupled", component_path.filename().string());
            throw std::runtime_error(e.what());
        }
    }

    driver::~driver(){
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
                return _componentImpl->on_loop();
            }
        }
        catch(const std::runtime_error& e){
            console::error("Runtime Error : {}", e.what());
        }
    }

    void driver::on_close(){
        try {
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

} /* namespace */