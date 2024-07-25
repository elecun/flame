
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
            thread worker = thread(&basler_gige_cam_linker::_image_stream_task, this, camera.first, camera.second, get_profile()->parameters());
            _camera_grab_worker[camera.first] = worker.native_handle();
            worker.detach();
        }

        thread monitor = thread(&basler_gige_cam_linker::_status_monitor_task, this, get_profile()->parameters());

    }
    catch(const GenericException& e){
        console::error("[{}] Pylon Generic Exception : {}", get_name(), e.GetDescription());
        return false;
    }
    catch(json::exception& e){
        console::error("Profile Error : {}", e.what());
        return false;
    }

    return true;
}

void basler_gige_cam_linker::on_loop(){

    

}

void basler_gige_cam_linker::on_close(){

    /* camera grab thread will be killed */
    for(auto& worker:_camera_grab_worker){
        pthread_cancel(worker.second);
        pthread_join(worker.second, nullptr);
    }
    _camera_grab_worker.clear();

    /* camera close and delete */
    for(auto& camera:_cameras){
        if(camera.second->IsOpen()){
            camera.second->StopGrabbing();
            camera.second->Close();
            delete camera.second;
        }
        
    }

    PylonTerminate();
}

void basler_gige_cam_linker::on_message(){
    
}

void basler_gige_cam_linker::_status_monitor_task(json parameters){
    while(!_thread_stop_signal.load()){
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void basler_gige_cam_linker::_image_stream_task(int camera_id, CBaslerUniversalInstantCamera* camera, json parameters){
    try{
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr);

        camera->Open();
        string acqusition_mode = parameters.value("acqusition_mode", "Continuous");
        string trigger_selector = parameters.value("trigger_selector", "FrameStart");
        string trigger_mode = parameters.value("trigger_mode", "On");
        string trigger_source = parameters.value("trigger_source", "Line2");
        string trigger_activation = parameters.value("trigger_activation", "RisingEdge");
        int heartbeat_timeout = parameters.value("heartbeat_timeout", 5000);

        camera->AcquisitionMode.SetValue(acqusition_mode.c_str());
        camera->TriggerSelector.SetValue(trigger_selector.c_str());
        camera->TriggerMode.SetValue(trigger_mode.c_str());
        camera->TriggerSource.SetValue(trigger_source.c_str());
        camera->TriggerActivation.SetValue(trigger_activation.c_str());
        camera->GevHeartbeatTimeout.SetValue(heartbeat_timeout);
        camera->StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByUser);
        CGrabResultPtr ptrGrabResult;

        while(camera->IsGrabbing() && !_thread_stop_signal.load()){
            try{
                camera->RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException); //trigger mode makes it blocked
                if(ptrGrabResult.IsValid()){
                    if(ptrGrabResult->GrabSucceeded()){
                        const uint8_t* pImageBuffer = (uint8_t*)ptrGrabResult->GetBuffer();
                        console::info("> camera {} captured : {}x{}", camera_id, ptrGrabResult->GetWidth(), ptrGrabResult->GetHeight());
                    }
                    else{
                        console::warn("[{}] Error-code({}) : {}", get_name(), ptrGrabResult->GetErrorCode(), ptrGrabResult->GetErrorDescription().c_str());
                    }
                }
                else
                    break;
            }
            catch(Pylon::TimeoutException& e){
                console::error("[{}] Camera {} Timeout exception occurred! {}", get_name(), camera_id, e.GetDescription());
                break;
            }
        }

        camera->StopGrabbing();
        camera->Close();
    }
    catch(const GenericException& e){
        console::error("[{}] {}", get_name(), e.GetDescription());
    }
}