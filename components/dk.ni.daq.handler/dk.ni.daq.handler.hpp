/**
 * @file dk.ni.daq.handler.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief NI DAQ Handler
 * @version 0.1
 * @date 2024-07-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_DK_NI_DAQ_HANDLER_HPP_INCLUDED
#define FLAME_DK_NI_DAQ_HANDLER_HPP_INCLUDED

#include <flame/component/object.hpp>

class dk_ni_daq_handler : public flame::component::object {
    public:
        dk_ni_daq_handler() = default;
        virtual ~dk_ni_daq_handler() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

}; /* class */

EXPORT_COMPONENT_API


#endif