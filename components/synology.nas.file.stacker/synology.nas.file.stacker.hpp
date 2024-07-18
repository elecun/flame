/**
 * @file synology.nas.file.stacker.hpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief File Stacker for Synology NAS Device
 * @version 0.1
 * @date 2024-06-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_SYNOLOGY_NAS_FILE_STACKER_HPP_INCLUDED
#define FLAME_SYNOLOGY_NAS_FILE_STACKER_HPP_INCLUDED

#include <flame/component/object.hpp>
#include <thread>
#include <string>
#include <condition_variable>
#include <unordered_map>
#include <queue>
#include <mutex>

using namespace std;


class synology_nas_file_stacker : public flame::component::object {
    public:
        synology_nas_file_stacker() = default;
        virtual ~synology_nas_file_stacker() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

    private:
        void _subscriber_callback(zmq::context_t& context, const string& topic);
        void _stacker_cakllback(const string& topic);


    private:
        thread* _subscriber { nullptr };
        thread* _stacker { nullptr };

        unordered_map<string, queue<string>> _sub_topics;
        unordered_map<string, mutex> _topic_mutex;
        unordered_map<string, condition_variable> _topic_csv;
        bool _thread_stop_signal { false };



}; /* class */

EXPORT_COMPONENT_API


#endif