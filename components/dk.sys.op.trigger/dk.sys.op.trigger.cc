
#include "dk.sys.op.trigger.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static dk_sys_op_trigger* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new dk_sys_op_trigger(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool dk_sys_op_trigger::on_init(){
    console::info("<{}> call dk_sys_op_trigger on_init", _THIS_COMPONENT_);

    //connect
    return true;
}

void dk_sys_op_trigger::on_loop(){
    console::info("<{}> call dk_sys_op_trigger on_loop", _THIS_COMPONENT_);

    static int n = 0;
    std::string message = fmt::format("push {}",n);
    zmq::message_t zmq_message(message.data(), message.size());
    this->get_dataport()->send(zmq_message, zmq::send_flags::dontwait);

    console::info("{} : {}", _THIS_COMPONENT_, message);

    n++;
}

void dk_sys_op_trigger::on_close(){
    console::info("<{}> call dk_sys_op_trigger on_close", _THIS_COMPONENT_);
}

void dk_sys_op_trigger::on_message(){
    console::info("<{}> call dk_sys_op_trigger on_message", _THIS_COMPONENT_);
}
