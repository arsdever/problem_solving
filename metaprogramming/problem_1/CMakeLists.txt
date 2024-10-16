get_filename_component(PROBLEM ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_executable(${SUBPROJECT}_${PROBLEM} main.cpp)
