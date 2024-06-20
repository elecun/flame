
#include "component.test.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

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

    static int n = 0;
    std::string message = fmt::format("push {}",n);
    zmq::message_t zmq_message(message.data(), message.size());
    this->get_dataport()->send(zmq_message, zmq::send_flags::dontwait);

    console::info("{}",message);

    n++;
}

void component_test::on_close(){
    console::info("<{}> call component_test on_close", _THIS_COMPONENT_);
}

void component_test::on_message(){
    console::info("<{}> call component_test on_message", _THIS_COMPONENT_);
}
