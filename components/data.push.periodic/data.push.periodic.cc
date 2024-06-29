
#include "data.push.periodic.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static data_push_periodic* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new data_push_periodic(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool data_push_periodic::on_init(){
    console::info("<{}> call data_push_periodic on_init", _THIS_COMPONENT_);
    return true;
}

void data_push_periodic::on_loop(){
    console::info("<{}> call data_push_periodic on_loop", _THIS_COMPONENT_);

    static int n = 0;
    std::string message = fmt::format("push {}",n);
    zmq::message_t zmq_message(message.data(), message.size());
    this->get_dataport()->send(zmq_message, zmq::send_flags::dontwait);

    console::info("{}",message);

    n++;
}

void data_push_periodic::on_close(){
    console::info("<{}> call data_push_periodic on_close", _THIS_COMPONENT_);
}

void data_push_periodic::on_message(){
    console::info("<{}> call data_push_periodic on_message", _THIS_COMPONENT_);
}
