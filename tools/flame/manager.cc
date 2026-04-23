
#include "manager.hpp"
#include <flame/def.hpp>
#include <flame/log.hpp>

// 'config' is defined as a macro in config.hpp, which conflicts with the 'flame::config' namespace.
// Undefine the macro here as this file does not need the singleton instance via the macro.
#undef config

#include <flame/config.hpp>

#if __cplusplus >= 202002L //c++20 (and later)
    #include <coroutine>
#endif

namespace flame {

    BundleManager::BundleManager() {

        // 1. create context
        component_uid_map_.reserve(50);
    }

    BundleManager::~BundleManager(){

        //1. call close function for all components, then clear the bundle container
        if(!bundle_container_.empty()){
            for(auto& [id, driver] : bundle_container_){
                if(driver)
                  driver->onClose();
            }
            bundle_container_.clear();
        }

    }

    bool BundleManager::install(fs::path bundle_repo){

        try{
            
            // 1. find component files and check profile existance in bundle repository
            vector<fs::path> component_list; 
            for(const auto& cfile : fs::directory_iterator(bundle_repo)){
                if(cfile.is_regular_file() && cfile.path().extension() == def::kComponentExt){ // .comp
                    fs::path pfile = cfile.path();
                    pfile.replace_extension(def::kProfileExt); // .profile
                    
                    if(fs::exists(pfile)){
                        component_list.push_back(pfile.replace_extension(""));
                    }
                }
            }
            logger::info("Found {} component(s) in the bundle.", component_list.size());

            /* bundle has own inproc context to share all components */
            // if(config->get_config().contains("inproc_context_io_threads")){
            //     int n_io_threads = config->get_config().value("inproc_context_io_threads",1);
            //     pipe_context flame::component::driver::inproc_pipeline_context = pipe_context(n_io_threads);
            //     logger::info("* In-process Context was created with {} IO threads", n_io_threads);
            // }

            // 2. check components profiles to create and manage inproc context
            for(auto& comp : component_list){
                string cname = comp.filename().string();
                component_uid_map_.insert(unordered_map<string, util::UuidT>::value_type(cname, uuid_gen_.generate()));
                bundle_container_.insert(BundleContainerT::value_type(component_uid_map_[cname], new component::Driver(comp)));

                logger::info("Installing component : {} (UID:{})", cname, component_uid_map_[cname].str());
            }

            // 3. call onInit function for all components
            for(auto& comp : component_list){
                string cname = comp.filename().string();
                if(!bundle_container_[component_uid_map_[cname]]->onInit()){
                    logger::error("<{}> component has a problem to initialize", cname);
                    this->uninstall(cname.c_str());
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
    
    void BundleManager::uninstall(const char* component_name){
        if(component_name!=nullptr){
            if(component_uid_map_.find(component_name)==component_uid_map_.end()){
                logger::warn("<{}> cannot be found in the repository", component_name);
                return;
            }

            bundle_container_[component_uid_map_[component_name]]->onClose(); // call onClose
            delete bundle_container_[component_uid_map_[component_name]]; // delete Driver instance
            bundle_container_.erase(component_uid_map_[component_name]); //erase in container
            component_uid_map_.erase(component_name); //erase in udi map
        }
        else {
            for(auto& [id, driver] : bundle_container_){
                if(driver){
                    logger::info("Uninstalling component <{}>", driver->getName());
                    driver->onClose();
                    delete driver;
                    driver = nullptr;
                }
            }
            bundle_container_.clear();
            component_uid_map_.clear();
        }
    }

    void BundleManager::startBundleService(){

        // call for all thread
        for(auto& [id, driver] : bundle_container_){
            if(driver)
              driver->onLoop();
        }
    }

    vector<string> BundleManager::getComponentList(){
        vector<string> list;
        for(auto& [id, driver] : bundle_container_){
            if(driver){
                list.push_back(driver->getName());
            }
        }
        return list;
    }

    json BundleManager::getComponentInfo(){
        json info = json::array();
        for(auto& [id, driver] : bundle_container_){
            if(driver){
                json c;
                c["name"] = driver->getName();
                c["type"] = driver->getType();
                c["status"] = driver->getStatusStr();
                info.push_back(c);
            }
        }
        return info;
    }

} /* namespace */