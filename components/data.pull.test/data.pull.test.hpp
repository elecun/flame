/**
 * @file data.pull.test.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component interface working test
 * @version 0.1
 * @date 2024-06-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_DATA_PULL_TEST_HPP_INCLUDED
#define FLAME_DATA_PULL_TEST_HPP_INCLUDED

#include <flame/component/object.hpp>


class data_pull_test : public flame::component::object {
    public:
        data_pull_test() = default;
        virtual ~data_pull_test() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

}; /* class */

EXPORT_COMPONENT_API


#endif