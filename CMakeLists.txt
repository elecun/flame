
cmake_minimum_required(VERSION 3.11)
project(
    flame_rt
    VERSION 0.0.1
    LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# build option examples
option(FLAME_RT_WITH_EXAMPLES "Enable building frame_rt examples" ON)
if(FLAME_RT_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()