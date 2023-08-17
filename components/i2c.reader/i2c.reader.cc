#include "i2c.reader.hpp"
#include <flame/log.hpp>
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

            //check profile keys requires
            vector<string> required_keys {"bus", "model", "chip_address", "conversion_register", "config_register", "configure"};
            for(string key:required_keys){
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

            // configuration
            unsigned char set_config[2] = {0x00, };
            set_config[0] = _i2c_set_configure & 0xff;
            set_config[1] = (_i2c_set_configure>>8) & 0xff;
            if((i2c_write(&_device, _i2c_config_register, set_config, sizeof(set_config)))!=sizeof(set_config)){
                console::error("I2C Set Error");
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

}

void i2c_reader::pause(){
    
}

void i2c_reader::resume(){
    
}

bool i2c_reader::open_i2c(const char* bus){
    _f_bus = open(bus, O_RDWR);
    if(_f_bus<0){
        console::error("Failed to open the I2C Bus {}", bus);
        return false;
    }
    return true;
}

long i2c_reader::read_i2c(const unsigned char address, int len){

    if(ioctl(_f_bus, I2C_SLAVE, _i2c_chip_address)<0){
        console::error("Failed to acquire bus access and/or talk to slave");
        return false;
    }

    return 0;
}