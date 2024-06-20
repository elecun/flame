/**
 * @file component.test2.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component interface working test
 * @version 0.1
 * @date 2024-06-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_COMPONENT_TEST2_HPP_INCLUDED
#define FLAME_COMPONENT_TEST2_HPP_INCLUDED

#include <flame/component/object.hpp>


class component_test2 : public flame::component::object {
    public:
        component_test2() = default;
        virtual ~component_test2() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

}; /* class */

EXPORT_COMPONENT_API


#endif