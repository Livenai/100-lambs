add_library(footbot_lamb_range_and_bearing MODULE
    footbot_lamb.h
    footbot_lamb.cpp)
target_link_libraries(footbot_lamb_range_and_bearing
  argos3core_simulator
  argos3plugin_simulator_footbot
  argos3plugin_simulator_genericrobot)
