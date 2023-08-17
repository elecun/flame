/**
 * @file i2c.reader.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief ADC data reader from I2C on RPi board
 * @version 0.1
 * @date 2023-08-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _FLAME_COMPONENT_I2C_READER_HPP_
#define _FLAME_COMPONENT_I2C_READER_HPP_

#include <flame/core.hpp>

#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>
#include <3rdparty/device/i2c.h>
#include <3rdparty/paho.mqtt/async_client.h> // for paho mqtt


using namespace flame;
using namespace std;
using namespace mqtt;

class i2c_reader : public core::task::runnable_rt{

    public:
        i2c_reader() = default;
        ~i2c_reader() = default;

        /* basic interface functions for nt */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

    private: //for MQTT
        mqtt::async_client* _mq_client = nullptr;
        

    private: //for I2C
        I2CDevice _device;

        string _i2c_dev_model = "";
        unsigned char _i2c_chip_address = 0x00;
        unsigned char _i2c_conversion_register = 0x00;
        unsigned char _i2c_config_register = 0x00;
        unsigned short _i2c_set_configure = 0x0000;
        int _f_bus = -1;
        

}; /* end class */

EXPORT_TASK_API

#endif