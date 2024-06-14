
#include "manager.hpp"
#include <flame/config_def.hpp>
#include <vector>
#include <flame/log.hpp>
#include <coroutine>

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
    }

    bool bundle_manager::install(fs::path repository){

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
            console::info("+ Load component : {}", _cname);

            _component_uid_map.insert(map<string, util::uuid_t>::value_type(_cname, _uuid_gen.generate()));
            _bundle_container.insert(map<util::uuid_t, component::driver*>::value_type(_component_uid_map[_cname], new component::driver(comp)));
        }

        // if(_task_container[_task_uid_map[taskname]]->good()){
        //     console::info("Successfully installed <{}>(UID:{})", taskname, _task_uid_map[taskname].str());
        //     if(!_task_container[_task_uid_map[taskname]]->configure()){
        //         console::error("<{}> component has a problem to configure.", taskname);
        //         this->uninstall(taskname);
        //     }
        // }
        // else {
        //     console::error("<{}> component load failed", taskname);
        //     this->uninstall(taskname);
        // }

        return true;
    }
    
    void bundle_manager::uninstall(){

    }

    void bundle_manager::start_bundle_service(){

        // call for all thread
        // for(bundle_container_t::iterator itr=_bundle_container.begin(); itr!=_bundle_container.end():++itr){
        //     itr->second->
        // }

        /* for all tasks */
        // if(!taskname){
        //     for(task_container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
        //         itr->second->execute();
        //     }
        // }
        // /* for specified task */
        // else {
        //     if(_task_uid_map.find(taskname)==_task_uid_map.end()){
        //         console::warn("{} cannot be found to run", taskname);
        //         return;
        //     }

        //     _task_container[_task_uid_map[taskname]]->execute();
        // }

    }

} /* namespace */