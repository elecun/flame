/**
 * @file simple.example.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Simple Example for testing
 * @version 0.1
 * @date 2023-07-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _FLAME_COMPONENT_SIMPLE_EXAMPLE_HPP_
#define _FLAME_COMPONENT_SIMPLE_EXAMPLE_HPP_

#include <flame/core.hpp>

using namespace flame;
using namespace std;

class simple_example : public core::task::runnable_rt {

    public:
        simple_example() = default;
        ~simple_example() = default;

        /* basic interface functions for nt */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;
        

}; /* end class */

EXPORT_TASK_API

#endif