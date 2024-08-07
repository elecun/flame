
#include <flame/core/manager.hpp>
#include <flame/log.hpp>
#include <flame/util/file.hpp>
#include <flame/core/registry.hpp>
#include <flame/core/def.hpp>
#include <flame/core/profile_def.hpp>
#include <filesystem>

namespace fs = std::filesystem;

namespace flame::core {

    task_manager::task_manager() {
        
    }

    task_manager::~task_manager(){
        if(_task_container.size()>0){
            for(task_container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                itr->second->cleanup();
            }
            _task_container.clear();
        }
    }

    bool task_manager::install(const char* taskname, flame::core::task::runnable* instance){

        /* static installation */
        if(instance){
            logger::warn("Static installation is not support yet");
            return false;
        }

        /* dynamic installation (~ install from component file) */
        else {
            if(!taskname){
                logger::warn("Task was not specified. It must be required.", taskname);
                return false;
            }

            // 1. check file existance
            if(registry->find(_PATH_BIN_DIR_)){
                fs::path _bin = registry->get<string>(_PATH_BIN_DIR_);
                fs::path _task = _bin / fs::path{fmt::format("{}{}",taskname, _FILE_EXT_COMPONENT_)};
                fs::path _profile = _bin / fs::path{fmt::format("{}{}",taskname, _FILE_EXT_PROFILE_)};

                if(!fs::exists(_task)){
                    logger::error("{}{} doest not exist. please check path or task file.", taskname, _FILE_EXT_COMPONENT_);
                    return false;
                }

                if(!fs::exists(_profile)){
                    logger::error("{}{} doest not exist. please check path or profile file.", taskname, _FILE_EXT_PROFILE_);
                    return false;
                }
            }

            // 2. check for already working
            if(_task_uid_map.find(taskname)!=_task_uid_map.end()){
                logger::error("{} is already installed. So it cannot be installed on same process.");
                return false;
            }

            // 3. insert into the container
            _task_uid_map.insert(map<string, util::uuid_t>::value_type(taskname, _uuid_gen.generate()));
            _task_container.insert(map<util::uuid_t, task::driver*>::value_type(_task_uid_map[taskname], new task::driver(taskname)));

            // 4. check load fail or success
            if(_task_container[_task_uid_map[taskname]]->good()){
                logger::info("Successfully installed <{}>(UID:{})", taskname, _task_uid_map[taskname].str());
                if(!_task_container[_task_uid_map[taskname]]->configure()){
                    logger::error("<{}> component has a problem to configure.", taskname);
                    this->uninstall(taskname);
                }
            }
            else {
                logger::error("<{}> component load failed", taskname);
                this->uninstall(taskname);
            }
            
        }

        return true;
    }

    void task_manager::uninstall(const char* taskname){
        /* for all tasks */
        if(!taskname){
            if(_task_container.size()>0){
                for(task_container_t::iterator itr = _task_container.begin(); itr!=_task_container.end(); ++itr){
                    itr->second->cleanup();
                    delete itr->second;
                }
                _task_container.clear();
                _task_uid_map.clear();
            }
        }
        /* for specified task */
        else {
            if(_task_uid_map.find(taskname)==_task_uid_map.end()){
                logger::warn("{} cannot be found to uninstall", taskname);
                return;
            }

            _task_container[_task_uid_map[taskname]]->cleanup();
            delete _task_container[_task_uid_map[taskname]];

            _task_container.erase(_task_uid_map[taskname]);
            _task_uid_map.erase(taskname);
        }
    }

    void task_manager::run(const char* taskname){
        /* for all tasks */
        if(!taskname){
            for(task_container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                itr->second->execute();
            }
        }
        /* for specified task */
        else {
            if(_task_uid_map.find(taskname)==_task_uid_map.end()){
                logger::warn("{} cannot be found to run", taskname);
                return;
            }

            _task_container[_task_uid_map[taskname]]->execute();
        }
        
    }

    void task_manager::cleanup(const char* taskname){
        /* for all tasks */
        if(!taskname){
            for(task_container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                itr->second->cleanup();
            }
        }
        /* for specified task */
        else {
            if(_task_uid_map.find(taskname)==_task_uid_map.end()){
                _task_container[_task_uid_map[taskname]]->cleanup();
            }
        }
    }

    flame::core::task::driver* task_manager::get_driver(const char* taskname){
        if(!taskname){
            logger::error("It must be specified.");
        }
        else{
            if(_task_uid_map.find(taskname)==_task_uid_map.end()){
                return _task_container[_task_uid_map[taskname]];
            }
        }
        return nullptr;
    }

} //namespace