
#include "driver.hpp"

#include <chrono>
#include <csignal>
#include <dlfcn.h>
#include <stdexcept>
#include <string>

#include <flame/def.hpp>
#include <flame/log.hpp>

using namespace std;

#if defined(linux) || defined(__linux) || defined(__linux__)
static int SIG_RUNTIME_TRIGGER = (SIGRTMIN);
#endif



namespace flame::component {

Driver::Driver(path component_path) : is_running_(false) {
  try {
    fs::path cobject = component_path.replace_extension(def::kComponentExt);
    fs::path cprofile = component_path.replace_extension(def::kProfileExt);

    if (load(cobject)) {
      assert(component_impl_ != nullptr);

      /* set important parameters and profile first */
      component_impl_->profile_ = make_unique<component::Profile>(cprofile);
      component_impl_->name_ = component_path.stem().string();
      component_impl_->status_ = DTypeStatus::kStopped;

      /* get dataport descriptions in profile */
      json _dataport = component_impl_->getProfile()->dataPort();

      /* assign context with number of io threads */
      logger::info("Component <{}> loaded dataport config",
                   component_path.filename().string());

      /* assign user defined data port */
      if (!_dataport.empty()) {
        for (auto &[portname, parameter] : _dataport.items()) {
          string socket_type = parameter["socket_type"].get<string>();
          string transport = parameter["transport"].get<string>();

          /* create port(socket) for inproc transport */
          if (!transport.compare("inproc")) {
            auto sock = component_impl_->createZSocket(portname, str2Type(socket_type), transport, portname, 0);
            if (sock) {
                sock->setMessageCallback([this](flame::pipe::ZData& data) {
                    this->onData(data);
                });
            }
          }

          /* create port(socket) for ipc, but not support yet */
          else if (!transport.compare("ipc")) {
            logger::warn("ipc transport is not supported yet.");
            auto sock = component_impl_->createZSocket(portname, str2Type(socket_type), transport, portname, 0);
            if (sock) {
                sock->setMessageCallback([this](flame::pipe::ZData& data) {
                    this->onData(data);
                });
            }
          }

          /* create port(socket) for tcp and multicast */
          else if (!transport.compare("tcp") || !transport.compare("epgm") || !transport.compare("pgm")) {
            int port = parameter.value("port", 5555);
            string host = parameter.value("host", "*");
            auto sock = component_impl_->createZSocket(portname, str2Type(socket_type), transport, host, port);
            if (sock) {
                sock->setMessageCallback([this](flame::pipe::ZData& data) {
                    this->onData(data);
                });
            }
          }
        }
      }
    }
  } catch (std::runtime_error &e) {
    logger::error("<{}> component driver cannot be coupled",
                  component_path.filename().string());
    throw std::runtime_error(e.what());
  }
}

Driver::~Driver() {
  /* stop thread */
  is_running_.store(false);
  if (ptr_thread_) {
    pthread_kill(ptr_thread_->native_handle(), signal_id_);
    if (ptr_thread_->joinable()) {
      ptr_thread_->join();
    }
    delete ptr_thread_;
    ptr_thread_ = nullptr;
  }

  try {
    /* clear all */
    component_impl_->closePort();

    /* pipeline termination */
  } catch (const zmq::error_t &e) {
    if (e.num() == ENOTSOCK || e.num() == ETERM) {
      logger::debug("Pipeline cleanup: {}", e.what());
    } else {
      logger::error("Pipeline error : {}", e.what());
    }
  } catch (const std::runtime_error &e) {
    logger::error("Runtime Error : {}", e.what());
  }

  /* unload component */
  unload();
}

bool Driver::onInit() {
  try {
    if (component_impl_) {
      return component_impl_->onInit();
    }
  } catch (const std::runtime_error &e) {
    logger::error("Runtime Error(OnInit) : {}", e.what());
  }

  return false;
}

void Driver::onLoop() {

  try {
    if (component_impl_) {
      if (component_impl_->getStatus() ==
          flame::component::DTypeStatus::kStopped) {
        unsigned long long _rtime = component_impl_->getProfile()
                                        ->raw()[def::kProfileRtCycleNs]
                                        .get<unsigned long long>();
        setRTTimer(_rtime);

        if (!ptr_thread_) {
          is_running_.store(true);
          ptr_thread_ = new thread{&flame::component::Driver::doCycle, this};
        }
      }
    }
  } catch (const std::runtime_error &e) {
    logger::error("Runtime Error(OnLoop) : {}", e.what());
  }
}

void Driver::onClose() {
  try {
    timer_delete(timer_id_); // delete timer
    if (component_impl_) {

      /* shutdown pipeline context, all socket will be closed */
      //component_impl_->pipeline_context->shutdown();

      /* call after pipeline context shutdown */
      return component_impl_->onClose();
    }
  } catch (const std::runtime_error &e) {
    logger::error("Runtime Error(OnClose) : {}", e.what());
  } catch (const zmq::error_t &e) {
    logger::error("Pipeline error : {}", e.what());
  }
}

void Driver::onData(flame::component::ZData& data) {
  if (component_impl_) {
    try {
      component_impl_->onData(data);
    } catch (const std::exception &e) {
      logger::error("Exception in OnData for component {}: {}", getName(),
                    e.what());
    }
  }
}

bool Driver::load(
    fs::path component_path) { // component path = *.comp absolute path
  try {
    // not exist
    if (!fs::exists(component_path)) {
      logger::error("<{}> component cannot be found.",
                    component_path.filename().string());
      return false;
    }

    component_handle_ =
        dlopen(component_path.string().c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!component_handle_) {
      logger::error("<{}> {}", component_path.filename().string(), dlerror());
      return false;
    }

    CreateComponentFunc pfcreate =
        (CreateComponentFunc)dlsym(component_handle_, "Create");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
      logger::error("<{}> component instance cannot be created",
                    component_path.filename().string());
      dlclose(component_handle_);
      component_handle_ = nullptr;
      return false;
    }

    // create instance
    component_impl_ = pfcreate();
  } catch (std::runtime_error &e) {
    logger::error("<{}> : {}", component_path.filename().string(), e.what());
  }

  return true;
}

// unload task component
void Driver::unload() {
  try {
    if (component_impl_) {
      ReleaseComponentFunc pfrelease =
          (ReleaseComponentFunc)dlsym(component_handle_, "Release");

      // release instance
      if (pfrelease)
        pfrelease();
      component_impl_ = nullptr;
    }

    if (component_handle_) {
      dlclose(component_handle_);
      component_handle_ = nullptr;
    }
  } catch (std::runtime_error &e) {
    logger::error("component unload failed");
  }
}

void Driver::setRTTimer(unsigned long long nsec) {

  /* Set and enable alarm */
  signal_event_.sigev_notify =
      SIGEV_SIGNAL; // notify for process, so all thread received the signal
  signal_id_ = SIG_RUNTIME_TRIGGER;
  signal_event_.sigev_signo = SIG_RUNTIME_TRIGGER++;
  signal_event_.sigev_value.sival_ptr = timer_id_;
  if (timer_create(CLOCK_REALTIME, &signal_event_, &timer_id_) == -1)
    logger::error("timer create error");
  logger::info("[{}] Assigned Trigger Signal ID : {}", getName(), signal_id_);

  const unsigned long long nano = (1000000000L);
  time_spec_.it_value.tv_sec = nsec / nano;
  time_spec_.it_value.tv_nsec = nsec % nano;
  time_spec_.it_interval.tv_sec = nsec / nano;
  time_spec_.it_interval.tv_nsec = nsec % nano;

  // start timer
  if (timer_settime(timer_id_, 0, &time_spec_, nullptr) == -1)
    logger::error("timer setting error");
}

void Driver::doCycle() {

  // signal set for threading
  sigset_t thread_sigmask;
  sigemptyset(&thread_sigmask);
  sigaddset(&thread_sigmask, signal_id_); // block SIG_RUNTIME_TRIGGER signal
  int _sig_no;

  while (is_running_.load()) {
    sigwait(&thread_sigmask, &_sig_no); // wait until receive signal
    if (_sig_no == signal_id_) {
      if (component_impl_) {
        component_impl_->onLoop();
      }
    }
  }
}

} // namespace flame::component