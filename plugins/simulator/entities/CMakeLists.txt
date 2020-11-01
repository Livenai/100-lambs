if(ARGOS_BUILD_FOR_SIMULATOR)
  set(ARGOS3_HEADERS_PLUGINS_TROUGH_SIMULATOR
    dynamics2d_trough_model.h
    # pointmass3d_trough_model.h
    # physx_trough_model.h
    trough_entity.h)
endif(ARGOS_BUILD_FOR_SIMULATOR)

if(ARGOS_BUILD_FOR_SIMULATOR)
  set(ARGOS3_SOURCES_PLUGINS_TROUGH
    ${ARGOS3_HEADERS_PLUGINS_TROUGH_SIMULATOR}
    dynamics2d_trough_model.cpp
    # pointmass3d_trough_model.cpp
    # physx_trough_model.cpp
    trough_entity.cpp)
  # Compile the graphical visualization only if the necessary libraries have been found
  if(ARGOS_COMPILE_QTOPENGL)
    set(ARGOS3_HEADERS_PLUGINS_TROUGH_SIMULATOR
      ${ARGOS3_HEADERS_PLUGINS_TROUGH_SIMULATOR}
        qtopengl_trough.h)
    set(ARGOS3_SOURCES_PLUGINS_TROUGH
      ${ARGOS3_SOURCES_PLUGINS_TROUGH}
      qtopengl_trough.h
      qtopengl_trough.cpp)
  endif(ARGOS_COMPILE_QTOPENGL)
endif(ARGOS_BUILD_FOR_SIMULATOR)


add_library(argos3plugin_${ARGOS_BUILD_FOR}_trough SHARED ${ARGOS3_SOURCES_PLUGINS_TROUGH})
target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_trough
    argos3plugin_${ARGOS_BUILD_FOR}_dynamics2d)
  # argos3plugin_${ARGOS_BUILD_FOR}_pointmass3d)
  # argos3plugin_${ARGOS_BUILD_FOR}_physx)
if(ARGOS_COMPILE_QTOPENGL)
  target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_trough argos3plugin_${ARGOS_BUILD_FOR}_qtopengl)
endif(ARGOS_COMPILE_QTOPENGL)


if(ARGOS_BUILD_FOR_SIMULATOR)
  install(FILES ${ARGOS3_HEADERS_PLUGINS_TROUGH_SIMULATOR}  DESTINATION include/argos3/plugins/simulator/entities/trough)
endif(ARGOS_BUILD_FOR_SIMULATOR)

install(TARGETS argos3plugin_${ARGOS_BUILD_FOR}_trough
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/argos3
  ARCHIVE DESTINATION lib/argos3)
