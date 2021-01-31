include_directories(${CMAKE_SOURCE_DIR}/controllers/galib_lamb)
include_directories(/usr/include/python3.8)


add_library(galib_lamb_loop_functions SHARED
            galib_lamb_loop_functions.h
            galib_lamb_loop_functions.cpp)

find_package(Boost REQUIRED COMPONENTS filesystem)
target_link_libraries(galib_lamb_loop_functions
                      argos3core_simulator
                      Boost::filesystem
                      python3.8)
