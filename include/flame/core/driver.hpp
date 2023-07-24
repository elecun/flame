/**
 * @file driver.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Component Driver with core interface
 * @version 0.1
 * @date 2023-07-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _FLAME_CORE_DRIVER_HPP_
#define _FLAME_CORE_DRIVER_HPP_

#include <flame/core/task.hpp>
#include <flame/core/profile.hpp>

#include <string>
#include <thread>
#include <signal.h>
#include <mutex>
#include <filesystem> // requried c++17

using namespace std;
namespace fs = std::filesystem;

namespace flame::core::task {

    //RT timer jitter data
    typedef struct _time_jitter_t {
        unsigned long long max {0};
        unsigned long long min {1000000000};
        void set(unsigned long long val){
            if(val>max) max=val;
            if(val<min) min=val;
        }   
    } time_jitter;

    class driver {
        public:
            driver(const char* taskname);
            driver(task::runnable* instance);
            driver(const fs::path component);
            virtual ~driver();

            bool configure();   /* drive a task to configure before execution */
            void execute();     /* drive a task to run periodically */
            void cleanup();     /* drive a task to terminate */
            void pause();       /* drive a task to pause*/
            void resume();      /* drive a paused task to resume */

            /* get the name of task */
            const char* get_name() {
                if(_taskImpl)
                    return _taskImpl->get_name();
                return nullptr;
            }

            bool good(){ return (_taskImpl)?true:false; }

        private:
            bool load(const char* taskname);    /* load task with by name, true is success  */
            void unload();  /* unload task */
            void do_process();  /* call the concrete task execution */

            //set task time spec. 
            void set_rt_timer(unsigned long long nsec);

        private:
            task::runnable* _taskImpl = nullptr;    //concrete implementation
            void* _task_handle = nullptr;   //for dl
            std::thread* _ptrThread = nullptr;
            std::mutex _mutex;
            timer_t _timer_id {0};
            struct sigevent _sig_evt;
            struct itimerspec _time_spec;
            _time_jitter_t _jitter;
            bool _overrun { false };
    };
} //namespace

#endif