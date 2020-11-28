include_directories(${CMAKE_SOURCE_DIR}/controllers/lamb)
add_library(lamb_loop_functions MODULE
            lamb_loop_functions.h
            lamb_loop_functions.cpp)

find_package(Boost REQUIRED COMPONENTS filesystem)
target_link_libraries(lamb_loop_functions
                      argos3core_simulator
                      Boost::filesystem)
