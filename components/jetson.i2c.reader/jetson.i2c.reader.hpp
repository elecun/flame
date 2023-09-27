/**
 * @file i2c.reader.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief ADC data reader from I2C on Jetson Xavier NX
 * @version 0.1
 * @date 2023-08-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _FLAME_COMPONENT_JETSON_I2C_READER_HPP_
#define _FLAME_COMPONENT_JETSON_I2C_READER_HPP_

#include <flame/core.hpp>
#include <3rdparty/device/i2c.h>
#include <flame/log.hpp>
#include <3rdparty/paho.mqtt/async_client.h> // for paho mqtt
#include <fstream>
#include <map>


using namespace flame;
using namespace std;
using namespace mqtt;

class mq_callback : public virtual mqtt::callback{
    public:
        void connection_lost(const string& cause) override {
            if(!cause.empty()){
                console::warn("Connection lost : {}", cause);
            }
        }

        void delivery_complete(mqtt::delivery_token_ptr tok) override {
            // cout << "\tDelivery complete for token: "
            //     << (tok ? tok->get_message_id() : -1) << endl;
        }
};

class jetson_i2c_reader : public core::task::runnable_rt{

    public:
        jetson_i2c_reader() = default;
        ~jetson_i2c_reader() = default;

        /* basic interface functions for nt */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

    private: //for MQTT
        mqtt::async_client* _mq_client = nullptr;

        string _mq_broker_address {""};
        string _mq_pub_topic_prefix {""};
        string _mq_client_id {""};
        mqtt::connect_options _mq_option;
        int _mq_qos {0};
        

    private: //for I2C
        I2CDevice _device;

        string _i2c_dev_model {""};
        unsigned char _i2c_chip_address = 0x00;
        unsigned char _i2c_conversion_register = 0x00;
        unsigned char _i2c_config_register = 0x00;
        map<string, unsigned short> _i2c_set_configures;
        unsigned int _fsr = {2048};
        int _f_bus = -1;

    private:
        std::ofstream _datalog;


        

}; /* end class */

EXPORT_TASK_API

#endif