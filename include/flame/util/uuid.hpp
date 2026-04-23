/**
 * @file uuid.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief UUID class
 * @version 0.1
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef FLAME_UTIL_UUID_HPP_INCLUDED
#define FLAME_UTIL_UUID_HPP_INCLUDED

#include <string>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cstdint>

namespace flame {
    namespace util {

        typedef struct UuidT {
            int8_t uuid[16];

            UuidT& operator=(const UuidT& src){
                memcpy(this->uuid, src.uuid, sizeof(src.uuid));
                return *this;
            }

            std::string str(){
                char tmp[17] = {0,};
                memcpy(tmp, this->uuid, sizeof(this->uuid));
                tmp[16] = '\0';
                return std::string(tmp);
            }

            bool operator<(const UuidT& rhs) const {
                char tmp1[sizeof(uuid)+1] = {0,}, tmp2[sizeof(uuid)+1] = {0,};
                memcpy(tmp1, this->uuid, sizeof(this->uuid)); tmp1[sizeof(uuid)] = '\0';
                memcpy(tmp2, rhs.uuid, sizeof(rhs.uuid));  tmp2[sizeof(uuid)] = '\0';
                return std::string(tmp1) < std::string(tmp2);
            }
        } uuid; //struct UuidT

        const char kUuidKey[] = {
                '0','1','2','3','4','5','6','7','8','9',
                'A','B','C','D','E','F','G','H','I','J',
                'K','L','M','N','O','P','Q','R','S','T',
                'U','V','W','X','Y','Z'
            };

        class UuidGenerator {
            public:
            UuidGenerator(){
                std::srand(static_cast<unsigned int>(std::time(0)));
            }
            ~UuidGenerator(){}

            UuidT generate(){
                for(unsigned long n=0;n<sizeof(UuidT);n++)
                    uuid_.uuid[n] = kUuidKey[std::rand()%sizeof(kUuidKey)];
                return uuid_;
            }
            private:
                UuidT uuid_;
        }; //class UuidGenerator

    } //namespace util
} //namespace

#endif