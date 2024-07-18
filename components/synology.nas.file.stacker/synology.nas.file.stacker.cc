
#include "synology.nas.file.stacker.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static synology_nas_file_stacker* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new synology_nas_file_stacker(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool synology_nas_file_stacker::on_init(){
    
    // read profile parameters
    try {
        
    }
    catch(json::exception& e){
        console::error("Profile is not completed. {}", e.what());
        return false;
    }

    // join in data pipelines
    
    

    //connect
    return true;
}

void synology_nas_file_stacker::on_loop(){

    //

    static int n = 0;
    std::string message = fmt::format("push {}",n);
    zmq::message_t zmq_message(message.data(), message.size());
    this->get_dataport()->send(zmq_message, zmq::send_flags::dontwait);

    console::info("{} : {}", _THIS_COMPONENT_, message);

    n++;
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