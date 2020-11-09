include_directories(${CMAKE_SOURCE_DIR}/plugins/simulator/entities)
add_library(footbot_lamb MODULE footbot_lamb.h footbot_lamb.cpp footbot_lamb_bt_nodes.cpp)
target_link_libraries(footbot_lamb
    argos3core_simulator
    argos3plugin_simulator_footbot
    argos3plugin_simulator_genericrobot)
