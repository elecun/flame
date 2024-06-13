
#include "manager.hpp"
#include <flame/config_def.hpp>
#include <vector>
#include <flame/log.hpp>

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

        // check component & profile
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

        for(auto& comp : _comp_list){
            string _cname = comp.filename().string();
            console::info("+ Load component : {}", _cname);

            _component_uid_map.insert(map<string, util::uuid_t>::value_type(_cname, _uuid_gen.generate()));
            _bundle_container.insert(map<util::uuid_t, component::driver*>::value_type(_component_uid_map[_cname], 
                                        new component::driver(comp.replace_extension(__COMPONENT_FILE_EXT__))));
        }

        return true;
    }
    
    void bundle_manager::uninstall(){

    }

} /* namespace */