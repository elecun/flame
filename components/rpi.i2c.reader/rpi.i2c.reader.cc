#include "rpi.i2c.reader.hpp"
#include <flame/log.hpp>


//static component instance that has only single instance
static rpi_i2c_reader* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new rpi_i2c_reader(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void rpi_i2c_reader::execute(){
    console::info("rpi3.i2c.reader execute");
}

void rpi_i2c_reader::stop(){

}

bool rpi_i2c_reader::configure(){
    try {
        const json& profile = this->get_profile()->raw();
    }
    catch(const json::exception& e){
        console::error("Profile read/access error : {}", e.what());
        return false;
    }

    return true;
}

void rpi_i2c_reader::cleanup(){

    /* mqtt connection close */
    this->mosqpp::mosquittopp::disconnect();
    this->mosqpp::mosquittopp::loop_stop();
    mosqpp::lib_cleanup();

}

void rpi_i2c_reader::pause(){
    
}

void rpi_i2c_reader::resume(){
    
}


void rpi_i2c_reader::on_connect(int rc){
    _mqtt_connected = true;

}
void rpi_i2c_reader::on_disconnect(int rc){
    _mqtt_connected = false;
}
void rpi_i2c_reader::on_publish(int mid){

}
void rpi_i2c_reader::on_message(const struct mosquitto_message* message){

    #define MAX_BUFFER_SIZE     1024
    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string topic(message->topic);
    string strmsg = buffer;
    delete []buffer;

    try{
        json msg = json::parse(strmsg);

    }
    catch(json::exception& e){
        console::error("Message Error : {}", e.what());
    }
    console::info("mqtt data({}) : {}",message->payloadlen, strmsg);

}
void rpi_i2c_reader::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void rpi_i2c_reader::on_unsubscribe(int mid){

}
void rpi_i2c_reader::on_log(int level, const char* str){

}
void rpi_i2c_reader::on_error(){
    
}