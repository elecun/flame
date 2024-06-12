/**
 * @file device.uvccam.multi.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief UVC Camera service component
 * @version 0.1
 * @date 2024-06-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_COMPONENT_DEVICE_UVCCAM_MULTI_HPP_INCLUDED
#define FLAME_COMPONENT_DEVICE_UVCCAM_MULTI_HPP_INCLUDED

#include <flame/log.hpp>
#include <flame/component.hpp>

using namespace flame;


class device_uvccam_multi final : public  flame::component {
    public:

    virtual void on_init() override;
    virtual void on_loop() override;
    virtual void on_close() override;

}; /* class */

EXPORT_COMPONENT_API


#endif