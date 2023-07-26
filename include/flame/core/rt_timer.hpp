/**
 * @file rt_timer.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Realtime Timer
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _FLAME_CORE_RT_TIMER_HPP_
#define _FLAME_CORE_RT_TIMER_HPP_

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>

using namespace std;

namespace flame {
    namespace core {

        #define SIG_RUNTIME_TRIGGER (SIGRTMAX)

        typedef enum timer_type_t : unsigned int {
            PERIODIC = 0,
        } timer_type;

        class rt_timer {
            public:
            rt_timer();
            virtual ~rt_timer();
            
            virtual void start(long nsec, timer_type type = timer_type::PERIODIC);
            virtual void stop();

            protected:
            virtual void call() = 0;

            private:
            timer_t _timer_id = 0;
            struct sigevent _sig_evt;
            struct sigaction _sig_act;
            struct itimerspec _time_spec;

            static void handler(int signo, siginfo_t* info, void* context) {
                (reinterpret_cast<rt_timer*>(info->si_value.sival_ptr))->call();
            }
        };

        class rt_task : public rt_timer {
            public:
                //rt_task runnable interface
                class runnable {
                    public:
                    virtual void run() = 0;
                };

                void regist_runnable(runnable& h){
                    _runnable = &h;
                }

                void release_runnable() {
                    _runnable = nullptr;
                }

                void call() {
                    printf("call\n");
                    if(_runnable)
                        _runnable->run();
                }
            private:
                runnable* _runnable = nullptr;
        };

    } //namespace core
} //namespace

#endif