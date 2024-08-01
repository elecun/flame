
#include "image.flow.handler.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;
using namespace cv;
namespace fs = std::filesystem;

static image_flow_handler* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new image_flow_handler(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool image_flow_handler::on_init(){

    
    return true;
}

void image_flow_handler::on_loop(){

}

void image_flow_handler::on_close(){

}

void image_flow_handler::on_message(){
    
}
