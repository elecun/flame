
#include "dk.image.push.unittest.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

#include <opencv2/opencv.hpp>
#include <filesystem>

using namespace flame;
using namespace cv;
namespace fs = std::filesystem;

static dk_image_push_unittest* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new dk_image_push_unittest(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool dk_image_push_unittest::on_init(){

    try {

        // get image path
        fs::path current_path = fs::current_path();
        string dirname = get_profile()->raw()["parameters"]["dirname"].get<string>();
        fs::path _image_path = current_path / fs::path(dirname);

        //find images
        for(const auto& entry : fs::directory_iterator(_image_path)){
            if(entry.is_regular_file() && entry.path().extension()==".PNG"){
                _files.emplace_back(entry.path().string());
                console::info("file : {}", entry.path().string());
            }
        }

        
    }
    catch (const fs::filesystem_error& e){
        console::error("{}", e.what());
    }


    // load sample image
    // cv::Mat image = cv::imread("1.png", IMREAD_GRAYSCALE);
    
    return true;
}

void dk_image_push_unittest::on_loop(){

    // transfer data
    // static int n = 0;
    // std::string message = fmt::format("push {}",n);
    // zmq::message_t zmq_message(message.data(), message.size());
    // this->get_dataport()->send(zmq_message, zmq::send_flags::dontwait);

    // console::info("{}",message);
    // n++;
}

void dk_image_push_unittest::on_close(){

}

void dk_image_push_unittest::on_message(){
    
}
