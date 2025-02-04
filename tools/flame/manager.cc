
#include "manager.hpp"
#include <flame/config_def.hpp>
#include <flame/log.hpp>

#if __cplusplus >= 202002L //c++20 (and later)
    #include <coroutine>
#endif

namespace flame {

    bundle_manager::bundle_manager() {

        // 1. create context
        _component_uid_map.reserve(50);
    }

    bundle_manager::~bundle_manager(){

        //1. call close function for all components, then clear the bundle container
        if(!_bundle_container.empty()){
            for(bundle_container_t::iterator itr = _bundle_container.begin(); itr != _bundle_container.end(); ++itr){
                itr->second->on_close();
            }
            _bundle_container.clear();
        }

    }

    bool bundle_manager::install(fs::path bundle_repo){

        try{
            
            // 1. find component files and check profile existance in bundle repository
            vector<fs::path> _component_list; 
            for(const auto& cfile : fs::directory_iterator(bundle_repo)){
                if(cfile.is_regular_file() && cfile.path().extension() == __COMPONENT_FILE_EXT__){
                    fs::path pfile = cfile.path();
                    pfile.replace_extension(__PROFILE_FILE_EXT__);
                    
                    if(fs::exists(pfile)){
                        _component_list.push_back(pfile.replace_extension(""));
                    }
                }
            }
            logger::info("Found {} component(s) in the bundle.", _component_list.size());

            // 2. check components profiles to create and manage inproc context
            for(auto& comp : _component_list){
                string _cname = comp.filename().string();
                _component_uid_map.insert(map<string, util::uuid_t>::value_type(_cname, _uuid_gen.generate()));
                _bundle_container.insert(map<util::uuid_t, component::driver*>::value_type(_component_uid_map[_cname], new component::driver(comp)));

                logger::info("Installing component : {} (UID:{})", _cname, _component_uid_map[_cname].str());
            }

            // 3. call on_init function for all components
            for(auto& comp : _component_list){
                string _cname = comp.filename().string();
                if(!_bundle_container[_component_uid_map[_cname]]->on_init()){
                    logger::error("<{}> component has a problem to initialize", _cname);
                    this->uninstall(_cname.c_str());
                }
            }
        }
        catch(std::runtime_error& e){
            logger::error("Error : {}", e.what());
            this->uninstall();
            return false;
        }

        return true;
    }
    
    void bundle_manager::uninstall(const char* component_name){
        if(component_name!=nullptr){
            if(_component_uid_map.find(component_name)==_component_uid_map.end()){
                logger::warn("<{}> cannot be found in the repository", component_name);
                return;
            }

            _bundle_container[_component_uid_map[component_name]]->on_close(); // call on_close
            delete _bundle_container[_component_uid_map[component_name]]; // delete driver instance
            _bundle_container.erase(_component_uid_map[component_name]); //erase in container
            _component_uid_map.erase(component_name); //erase in udi map
        }
        else {
            for(bundle_container_t::iterator itr = _bundle_container.begin(); itr!=_bundle_container.end(); ++itr){
                if(itr->second){
                    logger::info("Uninstalling component <{}>", itr->second->get_name());
                    itr->second->on_close();
                    delete itr->second;
                    itr->second = nullptr;
                }
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