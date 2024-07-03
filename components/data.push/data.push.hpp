/**
 * @file data.push.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Data push through the data port
 * @version 0.1
 * @date 2024-07-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_DATA_PUSH_HPP_INCLUDED
#define FLAME_DATA_PUSH_HPP_INCLUDED

#include <flame/component/object.hpp>


class data_push : public flame::component::object {
    public:
        data_push() = default;
        virtual ~data_push() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

}; /* class */

EXPORT_COMPONENT_API


#endif