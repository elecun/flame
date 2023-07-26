
#include "rt_timer.hpp"
#include "excode.hpp"
#include <stdio.h>

namespace flame {
    namespace core {

        rt_timer::rt_timer(){

            printf("rt_timer create");

            //create handler which catches the signal SIG
            _sig_act.sa_flags = SA_SIGINFO;
            _sig_act.sa_sigaction = handler;
            sigisemptyset(&_sig_act.sa_mask);
            if(sigaction(SIG_RUNTIME_TRIGGER, &_sig_act, nullptr)==-1)
                throw excode::OE_SIGACTION_FAIL;

            //create timer
            _sig_evt.sigev_notify = SIGEV_SIGNAL;
            _sig_evt.sigev_signo = SIG_RUNTIME_TRIGGER;
            _sig_evt.sigev_value.sival_ptr = this;
            if(timer_create(CLOCK_REALTIME, &_sig_evt, &_timer_id)==-1)
                throw excode::OE_TIMER_CREATE_FAIL;
        }

        rt_timer::~rt_timer(){
            stop();
            timer_delete(_timer_id);
            signal(SIG_RUNTIME_TRIGGER, SIG_IGN);
        }

        void rt_timer::start(long nsec, timer_type type) {
            switch(type){
                case timer_type::PERIODIC:
                    const long nano = (1000000000L);
                    _time_spec.it_value.tv_sec = nsec / nano;
                    _time_spec.it_value.tv_nsec = nsec % nano;
                    _time_spec.it_interval.tv_sec = nsec / nano;
                    _time_spec.it_interval.tv_nsec = nsec % nano;
                break;
            }

            if(timer_settime(_timer_id, 0, &_time_spec, nullptr)==-1)
                throw excode::OE_SETTIME_FAIL;
        }

        void rt_timer::stop(){
            printf("rt_timer stop");
            struct itimerspec _new_time_spec;
            _new_time_spec.it_value.tv_sec = 0;
            _new_time_spec.it_value.tv_nsec = 0;
            _new_time_spec.it_interval.tv_sec = 0;
            _new_time_spec.it_interval.tv_nsec = 0;
            timer_settime(_timer_id, 0, &_new_time_spec, &_time_spec);
        }


    } //namespace core
} //namespace oe