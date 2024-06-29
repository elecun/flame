/**
 * @file synology.nas.file.stacker.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief File Stacker for Synology NAS Device
 * @version 0.1
 * @date 2024-06-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_SYNOLOGY_NAS_FILE_STACKER_HPP_INCLUDED
#define FLAME_SYNOLOGY_NAS_FILE_STACKER_HPP_INCLUDED

#include <flame/component/object.hpp>


class synology_nas_file_stacker : public flame::component::object {
    public:
        synology_nas_file_stacker() = default;
        virtual ~synology_nas_file_stacker() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

}; /* class */

EXPORT_COMPONENT_API


#endif