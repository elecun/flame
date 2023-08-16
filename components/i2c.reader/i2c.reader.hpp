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


using namespace flame;
using namespace std;

class i2c_reader : public core::task::runnable_rt {

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

    private:
        bool open_i2c(const char* bus);
        long read_i2c(const unsigned char addresss, int len);

    private:
        I2CDevice _device;

        unsigned char _i2c_chip_address = 0x00;
        unsigned char _i2c_data_address = 0x00;
        unsigned short _i2c_set_configure = 0x0000;
        int _f_bus = -1;
        

}; /* end class */

EXPORT_TASK_API

#endif