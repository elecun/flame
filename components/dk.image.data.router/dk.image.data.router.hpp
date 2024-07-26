/**
 * @file dk.image.data.router.hpp
 * @author Byunghun hwang <bh.hwang@iae.re.kr>
 * @brief 
 * @version 0.1
 * @date 2024-07-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FLAME_DK_IMAGE_DATA_ROUTER_HPP_INCLUDED
#define FLAME_DK_IMAGE_DATA_ROUTER_HPP_INCLUDED

#include <flame/component/object.hpp>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;


class dk_image_data_router : public flame::component::object {
    public:
        dk_image_data_router() = default;
        virtual ~dk_image_data_router() = default;

        // default interface functions
        bool on_init() override;
        void on_loop() override;
        void on_close() override;
        void on_message() override;

    private:
        vector<string> _files;
        vector<cv::Mat> _container;
        
        zmq::context_t* _context;
        zmq::socket_t* _socket;

}; /* class */

EXPORT_COMPONENT_API


#endif