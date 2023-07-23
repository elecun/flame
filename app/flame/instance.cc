
#include "instance.hpp"
#include <flame/log.hpp>
#include <flame/sys/cpu.hpp>
#include <flame/core/profile.hpp>


using namespace std;

namespace flame::app {

    /**
     * @brief Flame executor initialization
     * 
     * @param conf_file Json formmated configuration file
     * @return true if initialize success
     * @return false initialize failed
     */
    bool initialize(const char* conf_file){

        /* system information summary */
        console::info("Process Id = {}", getpid());
        console::info("System CPUs = {}", get_nprocs());
        console::info("System Clock Ticks = {}", sysconf(_SC_CLK_TCK));

        profile_data config;
        try {
            if(!util::exist(conf_file)){
                spdlog::error("Configuration file does not exist");
                return false;
            }

            std::ifstream file(conf_file);
            file >> config;
        }
        catch(const json::exception& e){
            spdlog::error("Configuration file load failed : {}", e.what());
            return false;
        }
        catch(std::ifstream::failure& e){
            spdlog::error("Configuration file load failed : {}", e.what());
            return false;
        }

        return true;
    }

    void run(){
        
    }

    void cleanup(){
        
    }

} /** namespace */