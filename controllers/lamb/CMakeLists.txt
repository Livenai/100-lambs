include_directories(${CMAKE_SOURCE_DIR}/plugins/simulator/entities)
add_library(lamb MODULE lamb.h lamb.cpp lamb_bt_nodes.cpp)
target_link_libraries(lamb
    argos3core_simulator
    argos3plugin_simulator_lambbot
    argos3plugin_simulator_genericrobot)
