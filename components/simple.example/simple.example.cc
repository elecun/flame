#include "simple.example.hpp"
#include <flame/log.hpp>


//static component instance that has only single instance
static simple_example* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new simple_example(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void simple_example::execute(){
    console::info("execute");
}

void simple_example::stop(){
    console::info("stop");
}

bool simple_example::configure(){
    try {
        const json& profile = this->get_profile()->raw();
        console::info("configure");
    }
    catch(const json::exception& e){
        console::error("Profile read/access error : {}", e.what());
        return false;
    }

    return true;
}

void simple_example::cleanup(){
    console::info("cleanup");
}

void simple_example::pause(){
    console::info("pause");
}

void simple_example::resume(){
    console::info("resume");
}

