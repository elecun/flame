
#include "dk.ni.daq.handler.hpp"
#include <flame/log.hpp>
#include <flame/config_def.hpp>

using namespace flame;

static dk_ni_daq_handler* _instance = nullptr;
flame::component::object* create(){ if(!_instance) _instance = new dk_ni_daq_handler(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool dk_ni_daq_handler::on_init(){

    /* get info from profile */
    _daq_device_name = get_profile()->parameters().value("daq_device_name", "Dev1");
    _daq_counter_channel = get_profile()->parameters().value("daq_counter_channel", "ctr0");
    

    return true;
}

void dk_ni_daq_handler::on_loop(){
    

}

void dk_ni_daq_handler::on_close(){
    
}

void dk_ni_daq_handler::on_message(){
    
}


bool dk_ni_daq_handler::_pulse_generate(double freq, unsigned long long n_pulses){

    string channel = fmt::format("{}/{}", _daq_device_name, _daq_counter_channel);

    TaskHandle _daq_task_handle { nullptr };

    /* create task handle */
    if(!DAQmxCreateTask("", &_daq_task_handle)){
        console::error("[{}] Failed to create DAQ task", get_name());
        return false;
    }

    /* create config to generate pulse */
    if(!DAQmxCreateCOPulseChanFreq(_daq_task_handle, channel.c_str(),  // device_name / counter_channel
                                    "",               // Name of the virtual channel (optional)
                                    DAQmx_Val_Hz,     // units
                                    DAQmx_Val_Low,    // idle state
                                    0.0,              // initial delay
                                    freq,             // frequency
                                    0.5               // Duty cycle
    )){
        console::error("[{}] Failed to create pulse channel", get_name());
        DAQmxClearTask(_daq_task_handle);
        return false;
    }

    /* set timing for infinite pulse generation */
    // Note! Finite sample : DAQmx_Val_FiniteSamps
    // Note! Infinite sample : DAQmx_Val_ContSamps
    if(!DAQmxCfgImplicitTiming(_daq_task_handle, DAQmx_Val_FiniteSamps, n_pulses)){
        console::error("[{}] Failed to configure timing", get_name());
        DAQmxClearTask(_daq_task_handle);
        return false;
    }

    /* start to run task */
    if(!DAQmxStartTask(_daq_task_handle)){
        console::error("[{}] Failed to run the pulse generation", get_name());
        DAQmxClearTask(_daq_task_handle);
        return false;
    }

    error = DAQmxWaitUntilTaskDone(taskHandle, DAQmx_Val_WaitInfinitely);
    if (error) {
        std::cerr << "Error waiting for task to complete: " << error << std::endl;
    } else {
        std::cout << number_of_pulses << " pulses generated at " << frequency << " Hz." << std::endl;
    }

    // Clear the task
    DAQmxClearTask(taskHandle);

    return true;
}