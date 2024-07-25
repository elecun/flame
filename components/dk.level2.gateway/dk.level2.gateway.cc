
#include "dk.level2.gateway.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static dk_level2_gateway* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new dk_level2_gateway(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool dk_level2_gateway::on_init(){

    //connect
    return true;
}

void dk_level2_gateway::on_loop(){

}

void dk_level2_gateway::on_close(){
    
}

void dk_level2_gateway::on_message(){
    
}
