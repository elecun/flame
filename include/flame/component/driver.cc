
#include "driver.hpp"

#include <filesystem>
#include <csignal>
#include <dlfcn.h>
#include <chrono>
#include <stdexcept>

#include <flame/log.hpp>

namespace fs = std::filesystem;
using namespace std;

namespace flame::component {

    driver::driver(string component_path){
        console::info("{} driver", component_path);
    }

    driver::driver(flame::component::object* instance){

    }

    driver::driver(fs::path component_path){
        console::info("path : {}", component_path.string());
    }

    driver::~driver(){
        
    }

    void driver::on_init(){

    }

    void driver::on_loop(){

    }

    void driver::on_close(){

    }

    void driver::on_message(){
        try {

        }
        catch(const std::runtime_error& e){
            console::error("Runtime Error : {}", e.what());
        }
    }

} /* namespace */