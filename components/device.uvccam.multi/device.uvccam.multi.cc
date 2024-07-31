
#include "device.uvccam.multi.hpp"

static device_uvccam_multi* _instance = nullptr;
flame::component* create(){ if(!_instance) _instance = new device_uvccam_multi(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void device_uvccam_multi::on_init(){
    logger::info("on_init");
}

void device_uvccam_multi::on_loop(){
    logger::info("on_loop");
}

void device_uvccam_multi::on_close(){
    logger::info("on_close");
}

