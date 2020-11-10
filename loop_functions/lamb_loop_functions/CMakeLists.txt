include_directories(${CMAKE_SOURCE_DIR}/controllers/footbot_lamb)
add_library(lamb_loop_functions MODULE
  lamb_qtuser_functions.h
lamb_qtuser_functions.cpp)

target_link_libraries(lamb_loop_functions
  argos3core_simulator
  argos3plugin_simulator_entities
  argos3plugin_simulator_footbot
  argos3plugin_simulator_qtopengl
  ${ARGOS_QTOPENGL_LIBRARIES})
