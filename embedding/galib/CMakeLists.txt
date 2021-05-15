include_directories(${CMAKE_SOURCE_DIR}/controllers/galib_lamb)
add_executable(galib_lamb_launcher main.cpp)

target_link_libraries(galib_lamb_launcher
  ${GALIB_LIBRARIES}
  galib_lamb
  galib_lamb_loop_functions
  argos3core_simulator)
