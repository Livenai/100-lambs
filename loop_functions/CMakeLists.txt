include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(lamb_loop_functions)

if(ARGOS_COMPILE_QTOPENGL)
  add_subdirectory(lamb_qtuser_functions)
endif(ARGOS_COMPILE_QTOPENGL)
