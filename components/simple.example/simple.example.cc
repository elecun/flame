#include "simple.example.hpp"
#include <flame/log.hpp>


//static component instance that has only single instance
static simple_example* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new simple_example(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void simple_example::execute(){
    logger::info("execute");
}

void simple_example::stop(){
    logger::info("stop");
}

bool simple_example::configure(){
    try {
        const json& profile = this->get_profile()->raw();
        logger::info("configure");
    }
    catch(const json::exception& e){
        logger::error("Profile read/access error : {}", e.what());
        return false;
    }

    return true;
}

void simple_example::cleanup(){
    logger::info("cleanup");
}

void simple_example::pause(){
    logger::info("pause");
}

void simple_example::resume(){
    logger::info("resume");
}

