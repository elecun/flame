
#include "synology.nas.file.stacker.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>
#include <flame/component/port.hpp>

using namespace flame;

static synology_nas_file_stacker* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new synology_nas_file_stacker(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool synology_nas_file_stacker::on_init(){
    
    //connect
    return true;
}

void synology_nas_file_stacker::on_loop(){

    std::string str_message = "test message";
    pipe_data data(str_message.data(), str_message.size());
    this->get_port("status_out")->send(data, zmq::send_flags::dontwait);

    console::info("sent message");

}

void synology_nas_file_stacker::on_close(){
    
}

void synology_nas_file_stacker::on_message(){
    
}


void synology_nas_file_stacker::_subscriber_callback(zmq::context_t& context, const string& topic)
{
    
}

void synology_nas_file_stacker::_stacker_cakllback(const string& topic)
{

}