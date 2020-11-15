include_directories(${CMAKE_SOURCE_DIR}/controllers/footbot_lamb)
add_library(lamb_loop_functions MODULE
  lamb_loop_functions.h
lamb_loop_functions.cpp)

target_link_libraries(lamb_loop_functions
  argos3core_simulator)
