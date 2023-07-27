#include "rpi.i2c.reader.hpp"
#include <flame/log.hpp>
#include<sstream>


//static component instance that has only single instance
static rpi_i2c_reader* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new rpi_i2c_reader(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void rpi_i2c_reader::execute(){
    console::info("rpi3.i2c.reader execute");
}

void rpi_i2c_reader::stop(){

}

bool rpi_i2c_reader::configure(){
    try {
        const json& profile = this->get_profile()->raw();
    }
    catch(const json::exception& e){
        console::error("Profile read/access error : {}", e.what());
        return false;
    }

    return true;
}

void rpi_i2c_reader::cleanup(){


}

void rpi_i2c_reader::pause(){
    
}

void rpi_i2c_reader::resume(){
    
}

bool rpi_i2c_reader::open_i2c(const char* bus){
    _f_bus = open(bus, O_RDWR);
    if(_f_bus<0){
        console::error("Failed to open the I2C Bus {}", bus);
        return false;
    }
    return true;
}

long rpi_i2c_reader::read_i2c(const char* address, int len){

    stringstream convert(address);
    int _i2c_address_int = 0;
	convert >> std::hex >> _i2c_address_int;

    if(ioctl(_f_bus, I2C_SLAVE, _i2c_address_int)<0){
        console::error("Failed to acquire bus access and/or talk to slave");
        return false;
    }

    //----- READ BYTES -----
	// length = 4;			//<<< Number of bytes to read
	// if (read(file_i2c, buffer, length) != length)		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
	// {
	// 	//ERROR HANDLING: i2c transaction failed
	// 	printf("Failed to read from the i2c bus.\n");
	// }
	// else
	// {
	// 	printf("Data read: %s\n", buffer);
	// }

}