/**
 * @file object.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component Object (*.comp)
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef FLAME_COMPONENT_OBJECT_HPP_INCLUDED
#define FLAME_COMPONENT_OBJECT_HPP_INCLUDED

#include <filesystem>
#include <flame/component/interface.hpp>
#include <flame/component/profile.hpp>
#include <flame/component/port.hpp>
#include <memory>

using namespace std;
using path = std::filesystem::path;

namespace flame::component {
    enum class dtype_status : int { STOPPED=0, WORKING};

    class driver;
    class object  : public interface {
        friend class flame::component::driver;

        public:
            object() = default;
            virtual ~object() = default;

            const char* get_name() const { return _name.c_str(); }
            const dtype_status get_status() const { return _status; }

        protected:
            component::profile* get_profile() const { 
                return _profile.get();
            }

            pipe_socket* get_port(const string portname) {
                return _socket_map[portname];
            }


        private:
            void set_status(dtype_status s) { _status = s; }

            /**
             * @brief Create a pipe context
             * 
             * @param pipename pipe name
             * @return pipe_context* created context
             */
            pipe_context* create_pipe(const string pipename){
                if(_pipe_map.contains(pipename)){
                    logger::warn("{} pipe is already defined", pipename);
                    return nullptr;
                }
                else {
                    _pipe_map.insert(make_pair(pipename, new pipe_context(1)));
                }
                return _pipe_map[pipename];
            }

            /**
             * @brief Create a port object
             * 
             * @param pipe created pipe context
             * @param socket_name socket name to create 
             * @param socket_type socket type (e.g. pub, sub)
             * @param q_size custom queue size (default:1000)
             * @return pipe_socket* created pipe socket pointer
             */
            pipe_socket* create_port(pipe_context* pipe, const string socket_name, const flame::socket_type socket_type, int q_size, string filter_topic = ""){

                string addr = fmt::format("inproc://{}", socket_name);

                switch(socket_type){
                    case flame::socket_type::pub:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipe, zmq::socket_type::pub)));
                        _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                        _socket_map[socket_name]->bind(addr);
                    }
                    break;

                    case flame::socket_type::sub:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipe, zmq::socket_type::sub)));
                        _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::subscribe, filter_topic);
                        _socket_map[socket_name]->connect(addr);
                    }
                    break;

                    case flame::socket_type::push:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipe, zmq::socket_type::push)));
                        _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                        _socket_map[socket_name]->bind(addr);
                    }
                    break;

                    case flame::socket_type::pull:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipe, zmq::socket_type::pull)));
                        _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                        _socket_map[socket_name]->connect(addr);
                    }
                    break;
                    default:
                        logger::warn("Undefined socket type you use.");
                }
                logger::info("[{}] Created port({}) : {}", _name, static_cast<int>(socket_type), addr);
                return _socket_map[socket_name];
            }

            pipe_socket* create_port(pipe_context* pipe, const string socket_name, const string socket_type, int q_size, const string address, int port, string topic = ""){
                if(!socket_type.compare("sub")){
                    _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipe, zmq::socket_type::sub)));
                    _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                    _socket_map[socket_name]->set(zmq::sockopt::subscribe, topic);
                    _socket_map[socket_name]->connect(fmt::format("tcp://{}:{}", address, port));
                }
                else if(!socket_type.compare("pub")){
                    _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipe, zmq::socket_type::pub)));
                    _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                    // _socket_map[socket_name]->set(zmq::sockopt::sndbuf, q_size);
                    _socket_map[socket_name]->bind(fmt::format("tcp://{}:{}", address, port));
                    logger::info("[{}] Created port({}) : {}", _name, address, port);
                }
                else if(!socket_type.compare("push")){
                    _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipe, zmq::socket_type::push)));
                    _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                    // _socket_map[socket_name]->set(zmq::sockopt::sndbuf, q_size);
                    _socket_map[socket_name]->bind(fmt::format("tcp://{}:{}", address, port));
                }
                else if(!socket_type.compare("pull")){
                    _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipe, zmq::socket_type::pull)));
                    _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                    _socket_map[socket_name]->connect(fmt::format("tcp://{}:{}", address, port));
                }
                else {
                    logger::warn("Undefined socket type to create dataport");
                }
                
                
                return _socket_map[socket_name];
            }

            void destory_pipe(){
                for(const auto& pair : _socket_map){
                    pair.second->close();
                    delete pair.second;
                }

                for(const auto& pair : _pipe_map){
                    pair.second->close();
                    delete pair.second;
                }
            }

        private:
            dtype_status _status { dtype_status::STOPPED };
            string _name = {"noname"};
            unique_ptr<profile> _profile;

            unordered_map<string, pipe_socket*> _socket_map;
            unordered_map<string, pipe_context*> _pipe_map;
            
            
    }; /* class */

    typedef flame::component::object*(*create_component)(void);
    typedef void(*release_component)(void);

    #define EXPORT_COMPONENT_API extern "C" { flame::component::object* create(void); void release(void); }

} /* namespace */

#endif