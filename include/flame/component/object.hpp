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
            /* get component profile */
            component::profile* get_profile() const { 
                return _profile.get();
            }

            /* get port from portname */
            pipe_socket* get_port(const string portname) {
                if(_socket_map.contains(portname))
                    return _socket_map[portname];
                else
                    throw std::runtime_error(fmt::format("{} does not exist.", portname));
                return nullptr;
            }

        private:
            void set_status(dtype_status s) { _status = s; }

            /* create inproc port */
            pipe_socket* create_port_inproc(const string socket_name, const flame::socket_type sock_type, int q_size, string fileter_topic = ""){
                string endpoint = fmt::format("inproc://{}", socket_name);
                switch(sock_type){
                    /* pub pattern socket */
                    case flame::socket_type::pub:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::pub)));
                        _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->bind(endpoint);
                    }
                    break;

                    /* sub pattern socket */
                    case flame::socket_type::sub:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::sub)));
                        _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::subscribe, fileter_topic);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->connect(endpoint);
                    }
                    break;

                    /* push pattern socket */
                    case flame::socket_type::push:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::push)));
                        _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->bind(endpoint);
                    }
                    break;

                    /* pull pattern socket */
                    case flame::socket_type::pull:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::pull)));
                        _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->connect(endpoint);
                    }
                    break;

                    /* req pattern socket */
                    case flame::socket_type::req:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::req)));
                        _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->connect(endpoint);
                    }
                    break;
                    
                    /* req pattern socket */
                    case flame::socket_type::rep:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::rep)));
                        _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->bind(endpoint);
                    }
                    break;

                    default:
                        logger::warn("Unsupported socket type you use.");
                }

                logger::info("[{}] Created Inproc port({}) : {}", _name, static_cast<int>(sock_type), endpoint);
                return _socket_map[socket_name];
            }

            /* create tcp port */
            pipe_socket* create_port_tcp(const string socket_name, const flame::socket_type sock_type, int q_size, const string address, int port, string filter_topic=""){
                
                switch(sock_type){
                    case flame::socket_type::pub:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::pub)));
                        _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->bind(fmt::format("tcp://{}:{}", address, port));

                    }
                    break;

                    case flame::socket_type::sub:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::sub)));
                        _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::subscribe, filter_topic);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->connect(fmt::format("tcp://{}:{}", address, port));
                    }
                    break;

                    case flame::socket_type::push:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::push)));
                        _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->bind(fmt::format("tcp://{}:{}", address, port));
                    }
                    break;

                    case flame::socket_type::pull:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::pull)));
                        _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->set(zmq::sockopt::rcvtimeo,500);
                        _socket_map[socket_name]->connect(fmt::format("tcp://{}:{}", address, port));
                    }
                    break;

                    case flame::socket_type::req:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::req)));
                        _socket_map[socket_name]->set(zmq::sockopt::sndhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->connect(fmt::format("tcp://{}:{}", address, port));
                    }
                    break;

                    case flame::socket_type::rep:{
                        _socket_map.insert(make_pair(socket_name, new pipe_socket(*pipeline_context, zmq::socket_type::rep)));
                        _socket_map[socket_name]->set(zmq::sockopt::rcvhwm, q_size);
                        _socket_map[socket_name]->set(zmq::sockopt::linger, 0);
                        _socket_map[socket_name]->bind(fmt::format("tcp://{}:{}", address, port));
                    }
                    break;
                    default:
                        logger::warn("Unsupported socket type you use.");
                }
                
                logger::info("[{}] Created TCP port({}) : {}:{}", _name, static_cast<int>(sock_type), address, port);
                return _socket_map[socket_name];
            }

        /* clear all pipe data to close immediately */
        void close_port(){
            for(auto& [key, sock] : _socket_map){
                sock->set(zmq::sockopt::linger, 0);
                sock->close();
            }
        }

        private:
            dtype_status _status { dtype_status::STOPPED };
            string _name = {"noname"};
            unique_ptr<profile> _profile;

            unordered_map<string, pipe_socket*> _socket_map;

        protected:
            /* pipe context */
            unique_ptr<pipe_context> pipeline_context;
            
            
    }; /* class */

    typedef flame::component::object*(*create_component)(void);
    typedef void(*release_component)(void);

    #define EXPORT_COMPONENT_API extern "C" { flame::component::object* create(void); void release(void); }

} /* namespace */

#endif