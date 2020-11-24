#
# Foot-bot headers
#
# argos3/plugins/robots/lamb-bot/control_interface
set(ARGOS3_HEADERS_PLUGINS_ROBOTS_LAMBBOT_CONTROLINTERFACE
    control_interface/ci_lambbot_base_ground_sensor.h
    control_interface/ci_lambbot_distance_scanner_actuator.h
    control_interface/ci_lambbot_distance_scanner_sensor.h
    control_interface/ci_lambbot_encoder_sensor.h
    control_interface/ci_lambbot_gripper_actuator.h
    control_interface/ci_lambbot_light_sensor.h
    control_interface/ci_lambbot_motor_ground_sensor.h
    control_interface/ci_lambbot_proximity_sensor.h
    control_interface/ci_lambbot_turret_actuator.h
    control_interface/ci_lambbot_turret_encoder_sensor.h)
# argos3/plugins/robots/lamb-bot/simulator
if(ARGOS_BUILD_FOR_SIMULATOR)
  set(ARGOS3_HEADERS_PLUGINS_ROBOTS_LAMBBOT_SIMULATOR
    simulator/dynamics2d_lambbot_model.h
    simulator/pointmass3d_lambbot_model.h
    # simulator/physx_lambbot_model.h
    simulator/lambbot_base_ground_rotzonly_sensor.h
    simulator/lambbot_distance_scanner_default_actuator.h
    simulator/lambbot_distance_scanner_equipped_entity.h
    simulator/lambbot_distance_scanner_rotzonly_sensor.h
    simulator/lambbot_entity.h
    simulator/lambbot_gripper_default_actuator.h
    simulator/lambbot_light_rotzonly_sensor.h
    simulator/lambbot_motor_ground_rotzonly_sensor.h
    simulator/lambbot_proximity_default_sensor.h
    simulator/lambbot_turret_default_actuator.h
    simulator/lambbot_turret_encoder_default_sensor.h
    simulator/lambbot_turret_entity.h)
endif(ARGOS_BUILD_FOR_SIMULATOR)

#
# Lamb-bot sources
#
set(ARGOS3_SOURCES_PLUGINS_ROBOTS_LAMBBOT
    ${ARGOS3_HEADERS_PLUGINS_ROBOTS_LAMBBOT_CONTROLINTERFACE}
    control_interface/ci_lambbot_base_ground_sensor.cpp
    control_interface/ci_lambbot_distance_scanner_actuator.cpp
    control_interface/ci_lambbot_distance_scanner_sensor.cpp
    control_interface/ci_lambbot_encoder_sensor.cpp
    control_interface/ci_lambbot_gripper_actuator.cpp
    control_interface/ci_lambbot_light_sensor.cpp
    control_interface/ci_lambbot_motor_ground_sensor.cpp
    control_interface/ci_lambbot_proximity_sensor.cpp
    control_interface/ci_lambbot_turret_actuator.cpp
    control_interface/ci_lambbot_turret_encoder_sensor.cpp)

if(ARGOS_BUILD_FOR_SIMULATOR)
  set(ARGOS3_SOURCES_PLUGINS_ROBOTS_LAMBBOT
    ${ARGOS3_SOURCES_PLUGINS_ROBOTS_LAMBBOT}
    ${ARGOS3_HEADERS_PLUGINS_ROBOTS_LAMBBOT_SIMULATOR}
    simulator/dynamics2d_lambbot_model.cpp
    simulator/pointmass3d_lambbot_model.cpp
    # simulator/physx_lambbot_model.cpp
    simulator/lambbot_base_ground_rotzonly_sensor.cpp
    simulator/lambbot_distance_scanner_default_actuator.cpp
    simulator/lambbot_distance_scanner_equipped_entity.cpp
    simulator/lambbot_distance_scanner_rotzonly_sensor.cpp
    simulator/lambbot_entity.cpp
    simulator/lambbot_gripper_default_actuator.cpp
    simulator/lambbot_light_rotzonly_sensor.cpp
    simulator/lambbot_motor_ground_rotzonly_sensor.cpp
    simulator/lambbot_proximity_default_sensor.cpp
    simulator/lambbot_turret_default_actuator.cpp
    simulator/lambbot_turret_encoder_default_sensor.cpp
    simulator/lambbot_turret_entity.cpp)
  # Compile the graphical visualization only if the necessary libraries have been found
  if(ARGOS_COMPILE_QTOPENGL)
    set(ARGOS3_HEADERS_PLUGINS_ROBOTS_LAMBBOT_SIMULATOR
        ${ARGOS3_HEADERS_PLUGINS_ROBOTS_LAMBBOT_SIMULATOR}
        simulator/qtopengl_lambbot.h)
    set(ARGOS3_SOURCES_PLUGINS_ROBOTS_LAMBBOT
        ${ARGOS3_SOURCES_PLUGINS_ROBOTS_LAMBBOT}
        simulator/qtopengl_lambbot.h
        simulator/qtopengl_lambbot.cpp)
  endif(ARGOS_COMPILE_QTOPENGL)
endif(ARGOS_BUILD_FOR_SIMULATOR)

#
# Create lamb-bot plugin
#
add_library(argos3plugin_${ARGOS_BUILD_FOR}_lambbot SHARED ${ARGOS3_SOURCES_PLUGINS_ROBOTS_LAMBBOT})
target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_lambbot
  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot
  argos3plugin_${ARGOS_BUILD_FOR}_dynamics2d
  argos3plugin_${ARGOS_BUILD_FOR}_pointmass3d)
  # argos3plugin_${ARGOS_BUILD_FOR}_physx)
if(ARGOS_COMPILE_QTOPENGL)
  target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_lambbot argos3plugin_${ARGOS_BUILD_FOR}_qtopengl)
endif(ARGOS_COMPILE_QTOPENGL)

install(FILES ${ARGOS3_HEADERS_PLUGINS_ROBOTS_LAMBBOT_CONTROLINTERFACE} DESTINATION include/argos3/plugins/robots/lamb-bot/control_interface)

if(ARGOS_BUILD_FOR_SIMULATOR)
  install(FILES ${ARGOS3_HEADERS_PLUGINS_ROBOTS_LAMBBOT_SIMULATOR}      DESTINATION include/argos3/plugins/robots/lamb-bot/simulator)
endif(ARGOS_BUILD_FOR_SIMULATOR)

install(TARGETS argos3plugin_${ARGOS_BUILD_FOR}_lambbot
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/argos3
  ARCHIVE DESTINATION lib/argos3)
