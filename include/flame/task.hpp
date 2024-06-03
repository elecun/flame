/**
 * @file task.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Task header
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_TASK_HPP_INCLUDED
#define FLAME_TASK_HPP_INCLUDED

namespace flame {

    class taskBase{
        public:

        /**
         * @brief task initialization
         * 
         */
        virtual void on_init() = 0;

        /**
         * @brief task termination
         * 
         */
        virtual void on_destroy() = 0;

    }; /* task inteface */

    class Periodic : public taskBase {
        public:
        /**
         * @brief loop callback
         * 
         */
        virtual void on_loop() = 0;

    }; /* periodic task class */

    class nonPeriodic : public taskBase {
        public:
        virtual void on_call() = 0;
    }
    
    template<typename taskType>
    class taskImpl {
        public:


    }; /* typeImple class */


template <typename... Parts>
using taskImpl = Extends<Task, Parts...>;

} /* namespace */


 #endif