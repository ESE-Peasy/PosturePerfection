set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.16) # Required by TFL
project(PosturePerfection VERSION 0.1.0)

# Ensure all necessary libraries are included
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../../tensorflow_src")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../../tensorflow_src/tensorflow/lite")

# Set up TFL path
if(NOT TENSORFLOW_SOURCE_DIR)
  get_filename_component(TENSORFLOW_SOURCE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/../../tensorflow_src" ABSOLUTE)
endif()

# Add and build TFL
add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)

add_executable(PosturePerfection inference_core.cpp main.cpp)
target_link_libraries(PosturePerfection tensorflow-lite ${CMAKE_DL_LIBS})
