
#include "profile.hpp"
#include <flame/log.hpp>

using namespace std;

namespace flame::core {
    profile::profile(const char* path){
        try{
            std::ifstream file(path);
            file >> data;
            valid = true;
        }
        catch(const json::exception& e){
            data.clear();
            valid = false;
        }
    }
}   //namespace oe

