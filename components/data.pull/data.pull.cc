
#include "data.pull.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static data_pull* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new data_pull(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool data_pull::on_init(){
    logger::info("init {}", _THIS_COMPONENT_);
    
    return true;
}

void data_pull::on_loop(){

    // zmq::message_t message;
    // this->get_dataport()->recv(&message);
    // logger::info("Received : {}", message.data());
    logger::info("data_pull on_loop");
}

void data_pull::on_close(){

}

void data_pull::on_message(){
    
}
