
#include "dk.ni.daq.handler.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static dk_ni_daq_handler* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new dk_ni_daq_handler(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool dk_ni_daq_handler::on_init(){
    

    //connect
    return true;
}

void dk_ni_daq_handler::on_loop(){
    

    static int n = 0;
    std::string message = fmt::format("push {}",n);
    zmq::message_t zmq_message(message.data(), message.size());
    this->get_dataport()->send(zmq_message, zmq::send_flags::dontwait);

    console::info("{} : {}", _THIS_COMPONENT_, message);

    n++;
}

void dk_ni_daq_handler::on_close(){
    
}

void dk_ni_daq_handler::on_message(){
    
}
