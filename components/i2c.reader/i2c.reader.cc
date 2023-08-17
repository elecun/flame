#include "i2c.reader.hpp"
#include<sstream>
#include <vector>



//static component instance that has only single instance
static i2c_reader* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new i2c_reader(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void i2c_reader::execute(){
    if(_f_bus){
        unsigned char rcv_buffer[2] = {0x00, 0x00};

        if((i2c_read(&_device, _i2c_conversion_register, rcv_buffer, sizeof(rcv_buffer))) == sizeof(rcv_buffer)) {
            console::info(fmt::format("{},{}", rcv_buffer[0], rcv_buffer[1]));
        }
        
    }
    else {
        console::error("I2C Bus access error");
    }
    
}

void i2c_reader::stop(){

}

bool i2c_reader::configure(){
    try {
        const json& profile = this->get_profile()->raw();
        if(profile.contains(_PROFILE_CONFIGURATIONS_KEY_)){
            json config = profile[_PROFILE_CONFIGURATIONS_KEY_];

            //check configuration keys requires
            vector<string> required_conf_keys {"bus", "model", "chip_address", "conversion_register", "config_register", "configure"};
            for(string key:required_conf_keys){
                if(!config.contains(key)){
                    console::error(fmt::format("'{}' should be defined in this profile", key));
                    return false;
                }
            }
        
            string bus_name = config["bus"].get<string>();
            if((_f_bus = i2c_open(bus_name.c_str())) == -1){
                console::error("I2C Device open failed");
                return false;
            }

            _i2c_dev_model = config["model"].get<string>();
            console::info(fmt::format("> Model Name : {}", _i2c_dev_model));

            string c_address = config["chip_address"].get<string>();
            _i2c_chip_address = (unsigned char)stoi(c_address, nullptr, 16);
            console::info(fmt::format("> I2C Chip Address : {}", c_address));

            string conv_reg_address = config["conversion_register"].get<string>();
            _i2c_conversion_register = (unsigned char)stoi(conv_reg_address, nullptr, 16);
            console::info(fmt::format("> I2C Conversion Register Address : {}", (int)_i2c_conversion_register));

            string config_reg_address = config["config_register"].get<string>();
            _i2c_config_register = (unsigned char)stoi(config_reg_address, nullptr, 16);
            console::info(fmt::format("> I2C Config Register Address : {}", (int)_i2c_config_register));

            string conf_value = config["configure"].get<string>();
            _i2c_set_configure = (unsigned short)stoi(conf_value, nullptr, 16);
            console::info(fmt::format("> I2C Configured : {}", conf_value));

            // I2C device initialize
            memset(&_device, 0, sizeof(_device));
            i2c_init_device(&_device);

            _device.bus = _f_bus;
            _device.addr = _i2c_chip_address;
            _device.iaddr_bytes = 1;
            _device.page_bytes = 16;

            // i2c configuration
            unsigned char set_config[2] = {0x00, };
            set_config[0] = _i2c_set_configure & 0xff;
            set_config[1] = (_i2c_set_configure>>8) & 0xff;
            if((i2c_write(&_device, _i2c_config_register, set_config, sizeof(set_config)))!=sizeof(set_config)){
                console::error("I2C Set Error");
            }
        }

        //for mqtt configuration
        if(profile.contains("mqtt")){
            json mqtt_config = profile["mqtt"];

            //mqtt configuration
            vector<string> required_mqtt_keys {"broker", "id", "qos", "pub_prefix"};
            for(string key:required_mqtt_keys){
                if(!mqtt_config.contains(key)){
                    console::error(fmt::format("'{}' should be defined in this profile", key));
                    return false;
                }
            }

            _mq_broker_address = mqtt_config["broker"].get<string>();
            _mq_client_id = mqtt_config["id"].get<string>();
            _mq_qos = mqtt_config["qos"].get<int>();
            _mq_pub_topic_prefix = mqtt_config["pub_prefix"].get<string>();

            try{
                if(!_mq_client)
                    _mq_client = new mqtt::async_client(_mq_broker_address, _mq_client_id, "./persist");
            }
            catch(const mqtt::exception& e){
                console::error("{}", e.what());
                return false;
            }

        }

    }
    catch(const json::exception& e){
        console::error("Profile read/access error : {}", e.what());
        return false;
    }

    return true;
}

void i2c_reader::cleanup(){
    i2c_close(_f_bus);

    if(_mq_client->is_connected()){
        _mq_client->disconnect()->wait();
        delete _mq_client;
    }

}

void i2c_reader::pause(){
    
}

void i2c_reader::resume(){
    
}
