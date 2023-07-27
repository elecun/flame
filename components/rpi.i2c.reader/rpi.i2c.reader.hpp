/**
 * @file rpi.i2c.reader.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief ADC data reader from I2C on RPi board
 * @version 0.1
 * @date 2023-07-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _FLAME_COMPONENT_RPI_I2C_READER_HPP_
#define _FLAME_COMPONENT_RPI_I2C_READER_HPP_

#include <flame/core.hpp>

#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>


using namespace flame;
using namespace std;

class rpi_i2c_reader : public core::task::runnable_rt {

    public:
        rpi_i2c_reader() = default;
        ~rpi_i2c_reader() = default;

        /* basic interface functions for nt */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

    private:
        bool open_i2c(const char* bus);
        long read_i2c(const char* addresss, int len);

    private:
        string _i2c_address = {""};
        int _f_bus = 0;
        

}; /* end class */

EXPORT_TASK_API

#endif