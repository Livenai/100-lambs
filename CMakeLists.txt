cmake_minimum_required(VERSION 2.8.12)
project(lamb)

# Deactivate RPATH for MacOSX
set(CMAKE_MACOSX_RPATH 0)
# Set CMake policies to select wanted behaviors
#
# Use new policies introduced up to this version
cmake_policy(VERSION 2.8.12)
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif(POLICY CMP0042)

#
# Set build options, compiler flags
set(CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(ARGoSBuildOptions)
include(ARGoSBuildFlags)
include(ARGoSBuildChecks)

include_directories(utility)

# Descend into the controllers directory
add_subdirectory(controllers)
add_subdirectory(loop_functions)
add_subdirectory(plugins)
# add_subdirectory(embedding)
