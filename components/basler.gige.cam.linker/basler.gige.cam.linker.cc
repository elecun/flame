
#include "basler.gige.cam.linker.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static basler_gige_cam_linker* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new basler_gige_cam_linker(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool basler_gige_cam_linker::on_init(){

    try{

        //1. initialization
        PylonInitialize();

        //2. find cameras
        CTlFactory& tlFactory = CTlFactory::GetInstance();
        DeviceInfoList_t devices;
        tlFactory.EnumerateDevices(devices);
        if(devices.size()>1)
            console::info("[{}] Found {} cameras", get_name(), devices.size());

        //3. create device
        for(int idx=0;idx<(int)devices.size();idx++){
            _cameras.insert(make_pair(stoi(devices[idx].GetUserDefinedName().c_str()), 
                            new CBaslerUniversalInstantCamera(tlFactory.CreateDevice(devices[idx]))));
            console::info("[{}] Found User ID {}, (SN:{}, Address : {})", get_name(), devices[idx].GetUserDefinedName().c_str(), devices[idx].GetSerialNumber().c_str(), devices[idx].GetIpAddress().c_str());
        }

        for(const auto& camera:_cameras){
            //_camera_grab_worker.emplace_back(new thread(&basler_gige_cam_linker::_image_stream_task, this, camera.first, camera.second));
            _camera_grab_worker.emplace_back(thread(&basler_gige_cam_linker::_image_stream_task, this, camera.first, camera.second));
        }

        // 3. read parameters
        json param_cameras = get_profile()->parameters()["cameras"];
        if(param_cameras.is_array()){
            for(const auto& cam: param_cameras){
                int id = cam["id"].get<int>();          //camera id (by user)
                string ip = cam["ip"].get<string>();    //ip address
                string sn = cam["sn"].get<string>();    //camera serial number
                //console::info("+ {}, {}, {}", id, ip, sn);
            }
        }

    }
    catch(const GenericException& e){
        console::error("[{}] Pylon Generic Exception : {}", get_name(), e.GetDescription());
        return false;
    }
    catch(json::exception& e){
        console::error("Profile Error : {}", e.what());
        return false;
    }

    

    // pylon initialization
    // PylonInitialize();

    // try {

    //     //finding camera device
    //     CTlFactory& tlFactory = CTlFactory::GetInstance();
    //     DeviceInfoList_t devices;
    //     tlFactory.EnumerateDevices(devices);
    //     console::info("Found {} cameras", devices.size());
    //     if(devices.size()<1){
    //         throw RUNTIME_EXCEPTION("No camera found!");
    //     }

    //     //create device
    //     for(int idx=0;idx<(int)devices.size();idx++){
    //         _cameras.insert(make_pair(devices[idx].GetSerialNumber(), new CInstantCamera(tlFactory.CreateDevice(devices[idx]))));
    //         console::info("Use {}({})", devices[idx].GetFriendlyName().c_str(), devices[idx].GetIpAddress().c_str());
    //     }

    //     //camera open & configure hardware trigger
    //     for(const auto& camera:_cameras){
    //         camera.second->Open();
            
    //         /*
    //         Hardware Triggering Setting parameters

    //         TriggerSelector = FrameStart
    //         TriggerMode = On
    //         TriggerActivation = RisingEdge
    //         TriggerSource = Line1
    //         */

    //         CEnumParameter(camera.second->GetNodeMap(), "TriggerSelector").SetValue("FrameStart");
    //         CEnumParameter(camera.second->GetNodeMap(), "TriggerMode").SetValue("On");
    //         CEnumParameter(camera.second->GetNodeMap(), "TriggerSource").SetValue("Line1");
    //         CEnumParameter(camera.second->GetNodeMap(), "TriggerActivation").SetValue("RisingEdge");

    //     }
    // }
    // catch(const GenericException& e){
    //     console::error("Pylon Generic Exception : {}", e.GetDescription());
    //     return false;
    // }

    //connect
    return true;
}

void basler_gige_cam_linker::on_loop(){

    // static int n = 0;
    // std::string message = fmt::format("push {}",n);
    // zmq::message_t zmq_message(message.data(), message.size());
    // this->get_dataport()->send(zmq_message, zmq::send_flags::dontwait);

    // console::info("{} : {}", _THIS_COMPONENT_, message);

    // n++;
}

void basler_gige_cam_linker::on_close(){

    _thread_stop_signal.store(true);

    //camera close
    for(auto& camera: _cameras){
        delete camera.second;
    }

    // pylob cleanup
    PylonTerminate();
}

void basler_gige_cam_linker::on_message(){
    
}


void basler_gige_cam_linker::_image_stream_task(int camera_id, CBaslerUniversalInstantCamera* camera){
    try{
        camera->Open();
        camera->AcquisitionMode.SetValue("Continuous");
        camera->StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByUser);
        CGrabResultPtr ptrGrabResult;

        while(camera->IsGrabbing() && !_thread_stop_signal.load()){
            camera->RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if(ptrGrabResult->GrabSucceeded()){
                const uint8_t* pImageBuffer = (uint8_t*)ptrGrabResult->GetBuffer();
                console::info("> camera {} captured : {}x{}", camera_id, ptrGrabResult->GetWidth(), ptrGrabResult->GetHeight());
            }
            else{
                console::warn("[{}] Error-code({}) : {}", get_name(), ptrGrabResult->GetErrorCode(), ptrGrabResult->GetErrorDescription().c_str());
            }
        }

        camera->StopGrabbing();
        camera->Close();

        console::info("camera {} is closed", camera_id);
    }
    catch(const GenericException& e){
        console::error("[{}] {}", get_name(), e.what());
    }
}