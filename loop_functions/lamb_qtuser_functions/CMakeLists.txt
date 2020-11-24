include_directories(${CMAKE_SOURCE_DIR}/controllers/lamb)
add_library(lamb_qtuser_functions MODULE
  lamb_qtuser_functions.h
lamb_qtuser_functions.cpp)

target_link_libraries(lamb_qtuser_functions
  argos3core_simulator
  argos3plugin_simulator_entities
  argos3plugin_simulator_lambbot
  argos3plugin_simulator_qtopengl
  ${ARGOS_QTOPENGL_LIBRARIES})
