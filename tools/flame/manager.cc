
#include "manager.hpp"
#include <flame/config_def.hpp>
#include <vector>
#include <flame/log.hpp>

#if __cplusplus >= 202002L //c++20 (and later)
    #include <coroutine>
#endif

namespace flame {

    bundle_manager::bundle_manager() {
    }

    bundle_manager::~bundle_manager(){
        if(!_bundle_container.empty()){
            for(bundle_container_t::iterator itr = _bundle_container.begin(); itr != _bundle_container.end(); ++itr){
                itr->second->on_close();
            }
            _bundle_container.clear();
        }

        // clear dataport map
        for(dataport_ctx_map_t::iterator itr = _dp_ctx_map.begin(); itr!=_dp_ctx_map.end(); ++itr){
            itr->second->shutdown();
            delete itr->second;
        }

        // clear service port map
        for(serviceport_ctx_map_t::iterator itr = _sp_ctx_map.begin(); itr !=_sp_ctx_map.end(); ++itr){
            itr->second->shutdown();
            delete itr->second;
        }
    }

    bool bundle_manager::install(fs::path repository){

        try{
            // check component & profile existance
            vector<fs::path> _comp_list;
            for(const auto& cfile : fs::directory_iterator(repository)){
                if(cfile.is_regular_file() && cfile.path().extension() == __COMPONENT_FILE_EXT__){
                    fs::path pfile = cfile.path();
                    pfile.replace_extension(__PROFILE_FILE_EXT__);
                    
                    if(fs::exists(pfile)){
                        _comp_list.push_back(pfile.replace_extension(""));
                    }
                }
            }
            console::info("Found {} dependent component(s)", _comp_list.size());

            // component load
            for(auto& comp : _comp_list){
                string _cname = comp.filename().string();

                _component_uid_map.insert(map<string, util::uuid_t>::value_type(_cname, _uuid_gen.generate()));
                _dp_ctx_map.insert(map<string, zmq::context_t*>::value_type(_cname, new zmq::context_t(1)));
                _sp_ctx_map.insert(map<string, zmq::context_t*>::value_type(_cname, new zmq::context_t(1)));
                _bundle_container.insert(map<util::uuid_t, component::driver*>::value_type(_component_uid_map[_cname], new component::driver(comp, _dp_ctx_map[_cname])));

                console::info("+ Load component : {} (UID:{})", _cname, _component_uid_map[_cname].str());
            }

            // call initialization of all components
            for(auto& comp : _comp_list){
                string _cname = comp.filename().string();
                if(!_bundle_container[_component_uid_map[_cname]]->on_init()){
                    console::error("<{}> component has a problem to initialize", _cname);
                    this->uninstall(_cname.c_str());
                }
            }
        }
        catch(std::runtime_error& e){
            this->uninstall();
            return false;
        }

        return true;
    }
    
    void bundle_manager::uninstall(const char* component_name){
        if(component_name!=nullptr){
            if(_component_uid_map.find(component_name)==_component_uid_map.end()){
                console::warn("<{}> cannot be found in the repository", component_name);
                return;
            }

            _bundle_container[_component_uid_map[component_name]]->on_close(); // call on_close
            delete _bundle_container[_component_uid_map[component_name]]; // delete driver instance
            _bundle_container.erase(_component_uid_map[component_name]); //erase in container
            _component_uid_map.erase(component_name); //erase in udi map
        }
        else {
            for(bundle_container_t::iterator itr = _bundle_container.begin(); itr!=_bundle_container.end(); ++itr){
                itr->second->on_close();
                delete itr->second;
            }
            _bundle_container.clear();
            _component_uid_map.clear();
        }
    }

    void bundle_manager::start_bundle_service(){

        // call for all thread
        for(bundle_container_t::iterator itr=_bundle_container.begin(); itr!=_bundle_container.end();++itr){
            itr->second->on_loop();
        }
    }

} /* namespace */