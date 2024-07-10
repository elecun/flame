
#include "basler.gige.cam.linker.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static basler_gige_cam_linker* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new basler_gige_cam_linker(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool basler_gige_cam_linker::on_init(){
    console::info("<{}> call basler_gige_cam_linker on_init", _THIS_COMPONENT_);

    //get parameters from profile
    json param_cameras = get_profile()->raw()["cameras"];
    if(param_cameras.is_array()){
        for(const auto& cam: param_cameras){
            int id = cam["id"].get<int>();          //camera id (by user)
            string ip = cam["ip"].get<string>();    //ip address
            string sn = cam["sn"].get<string>();    //camera serial number
        }
    }

    // pylon initialization
    PylonInitialize();

    try {

        //finding camera device
        CTlFactory& tlFactory = CTlFactory::GetInstance();
        DeviceInfoList_t devices;
        tlFactory.EnumerateDevices(devices);
        console::info("Found {} cameras", devices.size());
        if(devices.size()<1){
            throw RUNTIME_EXCEPTION("No camera found!");
        }

        //create device
        for(int idx=0;idx<(int)devices.size();idx++){
            _cameras.insert(make_pair(devices[idx].GetSerialNumber(), new CInstantCamera(tlFactory.CreateDevice(devices[idx]))));
            console::info("Use {}({})", devices[idx].GetFriendlyName().c_str(), devices[idx].GetIpAddress().c_str());
        }

        //camera open & configure hardware trigger
        for(const auto& camera:_cameras){
            camera.second->Open();
            
            /*
            Hardware Triggering Setting parameters

            TriggerSelector = FrameStart
            TriggerMode = On
            TriggerActivation = RisingEdge
            TriggerSource = Line1
            */

            CEnumParameter(camera.second->GetNodeMap(), "TriggerSelector").SetValue("FrameStart");
            CEnumParameter(camera.second->GetNodeMap(), "TriggerMode").SetValue("On");
            CEnumParameter(camera.second->GetNodeMap(), "TriggerSource").SetValue("Line1");
            CEnumParameter(camera.second->GetNodeMap(), "TriggerActivation").SetValue("RisingEdge");

        }
    }
    catch(const GenericException& e){
        console::error("Pylon Generic Exception : {}", e.GetDescription());
        return false;
    }

    //connect
    return true;
}

void basler_gige_cam_linker::on_loop(){
    console::info("<{}> call basler_gige_cam_linker on_loop", _THIS_COMPONENT_);

    static int n = 0;
    std::string message = fmt::format("push {}",n);
    zmq::message_t zmq_message(message.data(), message.size());
    this->get_dataport()->send(zmq_message, zmq::send_flags::dontwait);

    console::info("{} : {}", _THIS_COMPONENT_, message);

    n++;
}

void basler_gige_cam_linker::on_close(){
    console::info("<{}> call basler_gige_cam_linker on_close", _THIS_COMPONENT_);

    //camera close
    for(auto& camera: _cameras){
        camera.second->Close();
        delete camera.second;
    }

    //terminate pylon
    PylonTerminate();
}

void basler_gige_cam_linker::on_message(){
    console::info("<{}> call data_pull_test on_message", _THIS_COMPONENT_);
}
