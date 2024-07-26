
#include "dk.image.data.router.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;
using namespace cv;
namespace fs = std::filesystem;

static dk_image_data_router* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new dk_image_data_router(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool dk_image_data_router::on_init(){

    
    return true;
}

void dk_image_data_router::on_loop(){

}

void dk_image_data_router::on_close(){

}

void dk_image_data_router::on_message(){
    
}
