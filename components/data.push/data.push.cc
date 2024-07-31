
#include "data.push.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static data_push* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new data_push(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool data_push::on_init(){
    logger::info("init {}", _THIS_COMPONENT_);
    return true;
}

void data_push::on_loop(){

    // transfer data
    static int n = 0;
    std::string message = fmt::format("push {}",n);
    zmq::message_t zmq_message(message.data(), message.size());
    this->get_dataport()->send(zmq_message, zmq::send_flags::dontwait);

    logger::info("{}",message);
    n++;
}

void data_push::on_close(){

}

void data_push::on_message(){
    
}
