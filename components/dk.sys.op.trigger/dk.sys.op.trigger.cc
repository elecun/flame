
#include "dk.sys.op.trigger.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static dk_sys_op_trigger* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new dk_sys_op_trigger(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool dk_sys_op_trigger::on_init(){
    

    return true;
}

void dk_sys_op_trigger::on_loop(){


}

void dk_sys_op_trigger::on_close(){
    
}

void dk_sys_op_trigger::on_message(){
    
}
