
#include "component.test.hpp"
#include <flame/log.hpp>

using namespace flame;

static component_test* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new component_test(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool component_test::on_init(){
    console::info("<{}> call component_test on_init", _THIS_COMPONENT_);
    return true;
}

void component_test::on_loop(){
    console::info("<{}> call component_test on_loop", _THIS_COMPONENT_);
}

void component_test::on_close(){
    console::info("<{}> call component_test on_close", _THIS_COMPONENT_);
}

void component_test::on_message(){
    console::info("<{}> call component_test on_message", _THIS_COMPONENT_);
}
