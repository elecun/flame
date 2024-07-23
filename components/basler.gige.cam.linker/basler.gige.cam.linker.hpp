/**
 * @file basler.gige.cam.linker.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief Basler Gigabit Ethernet Camera Capture by external trigger
 * @version 0.1
 * @date 2024-06-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*
Hardware Triggering Setting parameters

TriggerSelector = FrameStart
TriggerMode = On
TriggerActivation = RisingEdge
TriggerSource = Line1

*/

// Note! FrameEnd Triggering is ....

#ifndef FLAME_BASLER_GIGE_CAM_LINKER_HPP_INCLUDED
#define FLAME_BASLER_GIGE_CAM_LINKER_HPP_INCLUDED

#include <flame/component/object.hpp>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <thread>
#include <string>
#include <atomic>

using namespace std;
using namespace Pylon;
using namespace GenApi;

class basler_gige_cam_linker : public flame::component::object {
    public:
        basler_gige_cam_linker() = default;
        virtual ~basler_gige_cam_linker() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

    private:
        void _image_stream_task(int camera_id, CBaslerUniversalInstantCamera* camera);

    private:
        vector<thread> _camera_grab_worker;
        map<int, CBaslerUniversalInstantCamera*> _cameras;
        std::atomic<bool> _thread_stop_signal { false };



}; /* class */

EXPORT_COMPONENT_API


#endif