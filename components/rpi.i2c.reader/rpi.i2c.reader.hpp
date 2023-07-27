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
#include <flame/net.hpp>

using namespace flame;
using namespace std;

class rpi_i2c_reader : public core::task::runnable_rt, private mosqpp::mosquittopp {

    public:
        rpi_i2c_reader():mosqpp::mosquittopp(){};
        ~rpi_i2c_reader() = default;

        /* basic interface functions for nt */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

    private:
        //MQTT Callback functions
        virtual void on_connect(int rc) override;
		virtual void on_disconnect(int rc) override;
		virtual void on_publish(int mid) override;
		virtual void on_message(const struct mosquitto_message* message) override;
		virtual void on_subscribe(int mid, int qos_count, const int* granted_qos) override;
		virtual void on_unsubscribe(int mid) override;
		virtual void on_log(int level, const char* str) override;
		virtual void on_error() override;
        

    private:
        bool _mqtt_connected = false;
        string _broker_address { "127.0.0.1" };
        int _broker_port {1883};
        string _pub_topic = {"undefined"};
        int _pub_qos = 2;
        int _keep_alive = {60};
        vector<string> _sub_topics;
        

}; /* end class */

EXPORT_TASK_API

#endif