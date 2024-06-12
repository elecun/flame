
#include "device.uvccam.multi.hpp"

static device_uvccam_multi* _instance = nullptr;
flame::component* create(){ if(!_instance) _instance = new device_uvccam_multi(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void device_uvccam_multi::on_init(){
    console::info("on_init");
}

void device_uvccam_multi::on_loop(){
    console::info("on_loop");
}

void device_uvccam_multi::on_close(){
    console::info("on_close");
}

