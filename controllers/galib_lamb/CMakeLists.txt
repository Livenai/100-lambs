include_directories(${CMAKE_SOURCE_DIR}/plugins/simulator/entities)

add_library(galib_lamb SHARED
  galib_lamb.h galib_lamb.cpp galib_lamb_bt_nodes.cpp)

target_link_libraries(galib_lamb
    argos3core_simulator
    argos3plugin_simulator_lambbot
    argos3plugin_simulator_genericrobot)
