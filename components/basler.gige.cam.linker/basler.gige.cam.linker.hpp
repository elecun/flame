/**
 * @file basler.gige.cam.linker.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief 
 * @version 0.1
 * @date 2024-06-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_BASLER_GIGE_CAM_LINKER_HPP_INCLUDED
#define FLAME_BASLER_GIGE_CAM_LINKER_HPP_INCLUDED

#include <flame/component/object.hpp>


class basler_gige_cam_linker : public flame::component::object {
    public:
        basler_gige_cam_linker() = default;
        virtual ~basler_gige_cam_linker() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

}; /* class */

EXPORT_COMPONENT_API


#endif